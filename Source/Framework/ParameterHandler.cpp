#include "ParameterHandler.h"

#include "ParameterList.h"
#include "Synth.h"

namespace {
// Helper to create a range for Layout Creation
inline juce::NormalisableRange<float> makeRange(float min, float max,
                                                float step, float centre) {
  juce::NormalisableRange<float> r(min, max, step);
  if (centre != 0.0f)
    r.setSkewForCentre(centre);
  return r;
}
}  // namespace

// ------ Lifetime ------ //
ParameterHandler::ParameterHandler(juce::AudioProcessor& p)
    : processor(p),
      apvts(p, nullptr, "Parameters", createLayout()) {
  castAllParameters();
  apvts.state.addListener(this);
}

ParameterHandler::~ParameterHandler() {
  apvts.state.removeListener(this);
}

// ------ Pointer Layout ------ //
juce::AudioProcessorValueTreeState::ParameterLayout ParameterHandler::createLayout() {
  juce::AudioProcessorValueTreeState::ParameterLayout layout;

#define X(id, disp, mn, mx, st, def, ctr, lbl, path)          \
  layout.add(std::make_unique<juce::AudioParameterFloat>(     \
      ParameterID::id, disp, makeRange(mn, mx, st, ctr), def, \
      juce::AudioParameterFloatAttributes().withLabel(lbl)));
  PARAM_FLOAT_LIST
#undef X

#define X(id, disp, names, def, path)                      \
  layout.add(std::make_unique<juce::AudioParameterChoice>( \
      ParameterID::id, disp,                               \
      juce::StringArray::fromTokens(names, "|", {}), def));
  PARAM_CHOICE_LIST
#undef X

#define X(id, disp, def, path)                           \
  layout.add(std::make_unique<juce::AudioParameterBool>( \
      ParameterID::id, disp, def));
  PARAM_BOOL_LIST
#undef X

#define X(id, disp, mn, mx, def, path)                  \
  layout.add(std::make_unique<juce::AudioParameterInt>( \
      ParameterID::id, disp, mn, mx, def));
  PARAM_INT_LIST
#undef X

  return layout;
}

// ------ Pointer Casting ------ //

template <typename T>
inline static void castParameter(juce::AudioProcessorValueTreeState& apvts, const juce::ParameterID& id, T& destination) {
  destination = dynamic_cast<T>(apvts.getParameter(id.getParamID()));
  jassert(destination);  // parameter does not exist or wrong type
}

void ParameterHandler::castAllParameters() {
#define X(id, ...) castParameter(apvts, ParameterID::id, id##Param);
  PARAM_FLOAT_LIST
  PARAM_CHOICE_LIST
  PARAM_BOOL_LIST
  PARAM_INT_LIST
#undef X
}

// ------ Push Paramaters to Synth ------ //
void ParameterHandler::applyToSynth(Synth& synth) {
  // Columns: id, display, min, max, step, default, centre, unit, destination
#define X(id, disp, mn, mx, st, def, ctr, lbl, path) \
  synth.p.path = id##Param->get();
  PARAM_FLOAT_LIST
#undef X

  // Columns: id, display, names, default index, destination
#define X(id, disp, names, def, path) \
  synth.p.path = id##Param->getIndex();
  PARAM_CHOICE_LIST
#undef X

  // Columns: id, display, default, destination
#define X(id, disp, def, path) \
  synth.p.path = id##Param->get();
  PARAM_BOOL_LIST
#undef X

  // Columns: id, display, min, max, default, destination
#define X(id, disp, mn, mx, def, path) \
  synth.p.path = id##Param->get();
  PARAM_INT_LIST
#undef X
}