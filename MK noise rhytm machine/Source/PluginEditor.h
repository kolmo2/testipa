#pragma once

#include "PluginProcessor.h"
#include <array>
#include <memory>

class MKNoiseRhythmMachineEditor final : public juce::AudioProcessorEditor, private juce::Timer
{
public:
    explicit MKNoiseRhythmMachineEditor (MKNoiseRhythmMachineProcessor&);
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    class Panel final : public juce::Component
    {
        void paint (juce::Graphics&) override;
    };
    class GateGrid final : public juce::Component
    {
    public:
        explicit GateGrid (MKNoiseRhythmMachineProcessor& p) : processor (p) { setViewportIgnoreDragFlag (true); }
        void paint (juce::Graphics&) override;
        void mouseDown (const juce::MouseEvent&) override;
        void mouseDrag (const juce::MouseEvent&) override;
        void mouseUp (const juce::MouseEvent&) override;
    private:
        void setStepFromPosition (juce::Point<float>);
        juce::Rectangle<float> plotBounds() const;
        Pattern previewPattern { 1 };
        int selectedPoint = -1;
        MKNoiseRhythmMachineProcessor& processor;
    };

    void styleSlider (juce::Slider&);
    void timerCallback() override;
    MKNoiseRhythmMachineProcessor& processor;
    Panel panel;
    juce::Viewport viewport;
    juce::Label title, subtitle, layerHeading, effectsHeading, gateHeading;
    std::array<juce::Label, 4> gateControlLabels;
    std::array<juce::Label, 3> noiseSliderLabels;
    std::array<juce::Label, 3> layerLabels;
    std::array<juce::ComboBox, 3> noiseTypes;
    std::array<juce::ToggleButton, 3> voiceButtons;
    std::array<juce::Slider, 3> voiceLevels, voicePans, voiceTunes;
    std::array<std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment>, 3> typeAttachments;
    std::array<std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment>, 3> voiceAttachments;
    std::array<std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>, 3> levelAttachments,
        panAttachments, tuneAttachments;
    juce::ToggleButton gateEnabled, disableEffects;
    std::array<juce::ToggleButton, 10> effectButtons;
    std::array<juce::Slider, 10> effectKnobs;
    std::array<std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment>, 10> effectAttachments;
    std::array<std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>, 10> knobAttachments;
    juce::ComboBox gatePreset, gateCycle, gateShape, gateStereo;
    int lastSyncedPreset = -1;
    juce::ToggleButton gateFilter { "Filter Gate" };
    juce::Label presetDescription;
    juce::Label swingLabel;
    juce::ToggleButton gateRetrigger { "MIDI Retrigger" };
    juce::Slider gateSteps, gateAttack, gateRelease, gateTension, outputGain, gateSwing;
    juce::TextButton randomizeGate { "Randomize Gate" };
    GateGrid gateGrid;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> gateEnabledAttachment, retriggerAttachment, filterAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> cycleAttachment, shapeAttachment, stereoAttachment;
    std::unique_ptr<juce::ParameterAttachment> gateStepsAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        gateAttackAttachment, gateReleaseAttachment, tensionAttachment, outputGainAttachment, swingAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MKNoiseRhythmMachineEditor)
};
