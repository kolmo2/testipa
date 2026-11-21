#include "PluginEditor.h"
#include <limits>

namespace
{
const juce::StringArray noiseNames {
    "White Noise", "Pink Noise", "Vinyl 1930", "Vinyl 1950", "Vinyl 1960",
    "Mechanical", "Electrical"
};
constexpr const char* effectParameterIds[] = {
    "pitchOn", "phaserOn", "autoFilterOn", "chorusOn", "flangerOn",
    "modulationOn", "autoPanOn", "delayOn", "stereoShaperOn", "compressorOn"
};
constexpr const char* effectLabels[] = {
    "Pitch Shifter", "Phaser", "Auto Filter", "Chorus", "Flanger",
    "Modulation", "Auto Pan", "Delay", "Stereo Shaper", "Glue Comp"
};
}

MKNoiseRhythmMachineEditor::MKNoiseRhythmMachineEditor (MKNoiseRhythmMachineProcessor& p)
    : AudioProcessorEditor (&p), processor (p), gateGrid (p)
{
#if JUCE_IOS
    setSize (768, 820);
#else
    setSize (900, 982);
#endif
    title.setText ("MK NOISE RhYTM Machine", juce::dontSendNotification);
    title.setFont (juce::Font (juce::FontOptions (24.0f, juce::Font::bold)));
    title.setColour (juce::Label::textColourId, juce::Colour (0xffe6edf7));
    title.setJustificationType (juce::Justification::centredLeft);
    addAndMakeVisible (title);

    subtitle.setText ("THREE-LAYER NOISE INSTRUMENT  |  TEMPO-SYNCED TRANCE GATE",
                      juce::dontSendNotification);
    subtitle.setFont (juce::Font (juce::FontOptions (11.0f)));
    subtitle.setColour (juce::Label::textColourId, juce::Colour (0xff8fa3b8));
    addAndMakeVisible (subtitle);

    const auto addHeading = [this] (juce::Label& label, const juce::String& text)
    {
        label.setText (text, juce::dontSendNotification);
        label.setFont (juce::Font (juce::FontOptions (13.0f, juce::Font::bold)));
        label.setColour (juce::Label::textColourId, juce::Colour (0xffb5c9dd));
        addAndMakeVisible (label);
    };
    addHeading (layerHeading, "NOISE LAYERS");
    addHeading (effectsHeading, "EFFECTS  ·  GATE IS ALWAYS LAST");
    addHeading (gateHeading, "GATE12 TRANCE GATE  ·  DRAG CURVE POINTS");
    const juce::StringArray noiseControlNames { "Level", "Pan", "Pitch Shifter" };
    for (size_t i = 0; i < noiseSliderLabels.size(); ++i)
        addHeading (noiseSliderLabels[i], noiseControlNames[static_cast<int> (i)]);

    for (int i = 0; i < 3; ++i)
    {
        const auto suffix = juce::String (i + 1);
        layerLabels[static_cast<size_t> (i)].setText ("LAYER " + suffix, juce::dontSendNotification);
        layerLabels[static_cast<size_t> (i)].setColour (juce::Label::textColourId, juce::Colour (0xff94a9be));
        layerLabels[static_cast<size_t> (i)].setFont (juce::Font (juce::FontOptions (11.0f, juce::Font::bold)));
        addAndMakeVisible (layerLabels[static_cast<size_t> (i)]);

        voiceButtons[static_cast<size_t> (i)].setButtonText ("ON");
        addAndMakeVisible (voiceButtons[static_cast<size_t> (i)]);
        voiceAttachments[static_cast<size_t> (i)] =
            std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment> (
                processor.parameters, "voiceOn" + suffix, voiceButtons[static_cast<size_t> (i)]);

        for (int type = 0; type < noiseNames.size(); ++type)
            noiseTypes[static_cast<size_t> (i)].addItem (noiseNames[type], type + 1);
        addAndMakeVisible (noiseTypes[static_cast<size_t> (i)]);
        typeAttachments[static_cast<size_t> (i)] =
            std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment> (
                processor.parameters, "voiceType" + suffix, noiseTypes[static_cast<size_t> (i)]);

        styleSlider (voiceLevels[static_cast<size_t> (i)]);
        styleSlider (voicePans[static_cast<size_t> (i)]);
        styleSlider (voiceTunes[static_cast<size_t> (i)]);
        voiceLevels[static_cast<size_t> (i)].setTextValueSuffix (" dB");
        voicePans[static_cast<size_t> (i)].setTextValueSuffix ("");
        voiceTunes[static_cast<size_t> (i)].setTextValueSuffix (" st");
        addAndMakeVisible (voiceLevels[static_cast<size_t> (i)]);
        addAndMakeVisible (voicePans[static_cast<size_t> (i)]);
        addAndMakeVisible (voiceTunes[static_cast<size_t> (i)]);
        levelAttachments[static_cast<size_t> (i)] =
            std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
                processor.parameters, "voiceGain" + suffix, voiceLevels[static_cast<size_t> (i)]);
        panAttachments[static_cast<size_t> (i)] =
            std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
                processor.parameters, "voicePan" + suffix, voicePans[static_cast<size_t> (i)]);
        tuneAttachments[static_cast<size_t> (i)] =
            std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
                processor.parameters, "voiceTune" + suffix, voiceTunes[static_cast<size_t> (i)]);
    }

    for (int i = 0; i < 10; ++i)
    {
        effectButtons[static_cast<size_t> (i)].setButtonText (effectLabels[i]);
        addAndMakeVisible (effectButtons[static_cast<size_t> (i)]);
        effectAttachments[static_cast<size_t> (i)] =
            std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment> (
                processor.parameters, effectParameterIds[i], effectButtons[static_cast<size_t> (i)]);
        auto& knob = effectKnobs[static_cast<size_t> (i)];
        styleSlider (knob);
        knob.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
        knob.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 66, 20);
        knob.setTextValueSuffix (i == 0 ? " st" : "");
        knob.setName (juce::String (effectLabels[i]) + " Amount");
        addAndMakeVisible (knob);
        knobAttachments[static_cast<size_t> (i)] =
            std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
                processor.parameters, MKNoiseRhythmMachineProcessor::effectAmountParameterID (i), knob);
    }

    gateEnabled.setButtonText ("Gate On");
    addAndMakeVisible (gateEnabled);
    gateEnabledAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment> (
        processor.parameters, "gateOn", gateEnabled);
    disableEffects.setButtonText ("Disable All FX");
    disableEffects.onClick = [this]
    {
        for (const auto* id : effectParameterIds)
            if (auto* parameter = processor.parameters.getParameter (id))
                parameter->setValueNotifyingHost (0.0f);
        if (auto* parameter = processor.parameters.getParameter ("gateOn"))
            parameter->setValueNotifyingHost (0.0f);
    };
    addAndMakeVisible (disableEffects);

    for (int i = 0; i < processor.parameters.getParameter ("gatePreset")->getNumSteps(); ++i)
        gatePreset.addItem (processor.parameters.getParameter ("gatePreset")->getText (
                                static_cast<float> (i) / static_cast<float> (
                                    juce::jmax (1, processor.parameters.getParameter ("gatePreset")->getNumSteps() - 1)),
                                64), i + 1);
    gatePreset.setSelectedItemIndex (juce::roundToInt (
        processor.parameters.getRawParameterValue ("gatePreset")->load()), juce::dontSendNotification);
    gatePreset.onChange = [this]
    {
        if (gatePreset.getSelectedItemIndex() >= 0)
        {
            processor.applyGatePreset (gatePreset.getSelectedItemIndex());
            timerCallback();
        }
    };
    gatePreset.setViewportIgnoreDragFlag (true);
    addAndMakeVisible (gatePreset);

    styleSlider (gateSteps);
    styleSlider (gateAttack);
    styleSlider (gateRelease);
    styleSlider (gateTension);
    styleSlider (outputGain);
    styleSlider (gateSwing);
    gateSwing.setSliderStyle (juce::Slider::LinearVertical);
    gateSwing.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 64, 20);
    gateSwing.setTextValueSuffix (" %");
    gateSwing.setName ("Gate Swing");
    gateSwing.setTooltip ("Sixteenth-note swing: 0% is straight; 100% delays the offbeat to 2:1 timing");
    addHeading (swingLabel, "Swing");
    const juce::StringArray gateControlNames {
        "Attack", "Release", "Curve Tension", "Output"
    };
    for (int i = 0; i < static_cast<int> (gateControlLabels.size()); ++i)
    {
        auto& label = gateControlLabels[static_cast<size_t> (i)];
        label.setText (gateControlNames[i], juce::dontSendNotification);
        label.setFont (juce::Font (juce::FontOptions (10.0f, juce::Font::bold)));
        label.setColour (juce::Label::textColourId, juce::Colour (0xff8fa3b8));
        addAndMakeVisible (label);
    }
    gateSteps.setTextValueSuffix (" pts");
    gateAttack.setTextValueSuffix (" ms");
    gateRelease.setTextValueSuffix (" ms");
    outputGain.setTextValueSuffix (" dB");
    gateSteps.setTooltip ("Number of editable curve points, from 8 to 32");
    gateAttack.setTooltip ("Gate envelope attack time");
    gateRelease.setTooltip ("Gate envelope release time");
    for (int i = 0; i < 3; ++i)
    {
        voiceLevels[static_cast<size_t> (i)].setTooltip ("Noise layer level");
        voicePans[static_cast<size_t> (i)].setTooltip ("Noise layer stereo pan");
        voiceTunes[static_cast<size_t> (i)].setTooltip ("Real audio pitch shifting, -24 to +24 semitones");
    }
    for (auto* slider : { &gateSteps, &gateAttack, &gateRelease, &gateTension, &outputGain, &gateSwing })
        addAndMakeVisible (*slider);

    gateSteps.setRange (8, 32, 1);
    gateStepsAttachment = std::make_unique<juce::ParameterAttachment> (
        *processor.parameters.getParameter ("gateSteps"),
        [this] (float value) { gateSteps.setValue (value, juce::dontSendNotification); }, nullptr);
    gateStepsAttachment->sendInitialUpdate();
    gateSteps.onValueChange = [this] { processor.setGatePointCount (juce::roundToInt (gateSteps.getValue())); };
    gateAttackAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
        processor.parameters, "gateAttack", gateAttack);
    gateReleaseAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
        processor.parameters, "gateRelease", gateRelease);
    tensionAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
        processor.parameters, "gateTension", gateTension);
    outputGainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
        processor.parameters, "outputGain", outputGain);
    swingAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
        processor.parameters, "gateSwing", gateSwing);

    randomizeGate.onClick = [this] { processor.randomizeGate(); };
    addAndMakeVisible (randomizeGate);
    addAndMakeVisible (gateGrid);
    const auto populate = [this] (juce::ComboBox& box, const char* id)
    {
        auto* parameter = processor.parameters.getParameter (id);
        for (int i = 0; i < parameter->getNumSteps(); ++i)
            box.addItem (parameter->getText (parameter->convertTo0to1 (static_cast<float> (i)), 64), i + 1);
        addAndMakeVisible (box);
    };
    populate (gateCycle, "gateCycle");
    populate (gateShape, "gateShape");
    populate (gateStereo, "gateStereoMode");
    stereoAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment> (
        processor.parameters, "gateStereoMode", gateStereo);
    addAndMakeVisible (gateFilter);
    filterAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment> (
        processor.parameters, "gateFilterOn", gateFilter);
    presetDescription.setFont (juce::Font (juce::FontOptions (11.0f)));
    presetDescription.setColour (juce::Label::textColourId, juce::Colour (0xff8fa3b8));
    addAndMakeVisible (presetDescription);
    cycleAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment> (
        processor.parameters, "gateCycle", gateCycle);
    shapeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment> (
        processor.parameters, "gateShape", gateShape);
    gateCycle.setTooltip ("Length of one gate envelope cycle; quarter note equals one beat");
    gateShape.setTooltip ("Gate12 segment type; Curve with zero tension is linear");
    addAndMakeVisible (gateRetrigger);
    retriggerAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment> (
        processor.parameters, "gateRetrigger", gateRetrigger);
    gateRetrigger.setTooltip ("Restart the gate at each MIDI note-on; off follows host song position");
    while (getNumChildComponents() > 0)
        panel.addAndMakeVisible (getChildComponent (0));
    viewport.setViewedComponent (&panel, false);
    viewport.setScrollBarsShown (true, false);
    viewport.setScrollOnDragMode (juce::Viewport::ScrollOnDragMode::nonHover);
    addAndMakeVisible (viewport);
    resized();
    timerCallback();
    startTimerHz (30);
}

void MKNoiseRhythmMachineEditor::styleSlider (juce::Slider& slider)
{
    slider.setSliderStyle (juce::Slider::LinearHorizontal);
    slider.setViewportIgnoreDragFlag (true);
    slider.setTextBoxStyle (juce::Slider::TextBoxRight, false, 58, 20);
    slider.setColour (juce::Slider::trackColourId, juce::Colour (0xff35c4ce));
    slider.setColour (juce::Slider::thumbColourId, juce::Colour (0xffe7f2ff));
    slider.setColour (juce::Slider::textBoxTextColourId, juce::Colour (0xffdce7f3));
    slider.setColour (juce::Slider::textBoxBackgroundColourId, juce::Colour (0xff1a2532));
    slider.setColour (juce::Slider::textBoxOutlineColourId, juce::Colour (0xff35495e));
}

void MKNoiseRhythmMachineEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colour (0xff101720));
}

void MKNoiseRhythmMachineEditor::Panel::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colour (0xff101720));
    g.setColour (juce::Colour (0xff1c2936));
    g.fillRoundedRectangle (12.0f, 68.0f, static_cast<float> (getWidth() - 24), 183.0f, 10.0f);
    g.fillRoundedRectangle (12.0f, 260.0f, static_cast<float> (getWidth() - 24), 348.0f, 10.0f);
    g.fillRoundedRectangle (12.0f, 617.0f, static_cast<float> (getWidth() - 24), 350.0f, 10.0f);
    g.setColour (juce::Colour (0xff2b3b4d));
    g.drawRoundedRectangle (12.0f, 68.0f, static_cast<float> (getWidth() - 24), 183.0f, 10.0f, 1.0f);
    g.drawRoundedRectangle (12.0f, 260.0f, static_cast<float> (getWidth() - 24), 348.0f, 10.0f, 1.0f);
    g.drawRoundedRectangle (12.0f, 617.0f, static_cast<float> (getWidth() - 24), 350.0f, 10.0f, 1.0f);
    g.setColour (juce::Colour (0xff8fa3b8));
    g.setFont (juce::FontOptions (10.0f));
    g.drawText ("Gate12 curve engine by tiagolr / tilr - GPLv3", 24, 967, getWidth() - 48, 14,
                juce::Justification::centredRight);
}

void MKNoiseRhythmMachineEditor::resized()
{
    viewport.setBounds (getLocalBounds());
    panel.setSize (getWidth() - (getHeight() < 982 ? viewport.getScrollBarThickness() : 0), 982);
    const auto width = panel.getWidth();
    const auto contentWidth = width - 48;
    title.setBounds (22, 12, 550, 32);
    subtitle.setBounds (24, 43, 560, 18);
    layerHeading.setBounds (24, 72, 200, 20);
    effectsHeading.setBounds (24, 263, contentWidth - 150, 20);
    gateHeading.setBounds (24, 620, contentWidth, 20);

    for (int i = 0; i < 3; ++i)
    {
        const auto y = 112 + i * 42;
        const auto labelWidth = 56;
        const auto buttonWidth = 50;
        const auto typeWidth = contentWidth < 780 ? 130 : 150;
        const auto gap = 6;
        const auto sliderWidth = (contentWidth - labelWidth - buttonWidth - typeWidth - 5 * gap) / 3;
        auto x = 24;
        layerLabels[static_cast<size_t> (i)].setBounds (x, y + 5, labelWidth, 24);
        x += labelWidth + gap;
        voiceButtons[static_cast<size_t> (i)].setBounds (x, y + 2, buttonWidth, 28);
        x += buttonWidth + gap;
        noiseTypes[static_cast<size_t> (i)].setBounds (x, y + 2, typeWidth, 28);
        x += typeWidth + gap;
        if (i == 0)
            for (int control = 0; control < 3; ++control)
                noiseSliderLabels[static_cast<size_t> (control)].setBounds (
                    x + control * (sliderWidth + gap), 94, sliderWidth, 18);
        voiceLevels[static_cast<size_t> (i)].setBounds (x, y + 2, sliderWidth, 28);
        x += sliderWidth + gap;
        voicePans[static_cast<size_t> (i)].setBounds (x, y + 2, sliderWidth, 28);
        x += sliderWidth + gap;
        voiceTunes[static_cast<size_t> (i)].setBounds (x, y + 2, sliderWidth, 28);
    }

    const auto effectCellWidth = contentWidth / 5;
    for (int i = 0; i < 10; ++i)
    {
        const auto column = i % 5;
        const auto row = i / 5;
        effectButtons[static_cast<size_t> (i)].setBounds (24 + column * effectCellWidth,
                                                          290 + row * 154,
                                                          effectCellWidth - 8, 26);
        effectKnobs[static_cast<size_t> (i)].setBounds (
            24 + column * effectCellWidth, 316 + row * 154, effectCellWidth - 8, 124);
    }
    disableEffects.setBounds (width - 164, 263, 140, 24);
    gateEnabled.setBounds (24, 648, 90, 32);
    gatePreset.setBounds (120, 648, contentWidth - 286, 32);
    randomizeGate.setBounds (width - 184, 648, 160, 32);
    const auto gateCellWidth = contentWidth / 4;
    gateCycle.setBounds (24, 689, gateCellWidth - 8, 32);
    gateShape.setBounds (24 + gateCellWidth, 689, gateCellWidth - 8, 32);
    gateRetrigger.setBounds (24 + gateCellWidth * 2, 689, gateCellWidth - 8, 32);
    gateSteps.setBounds (24 + gateCellWidth * 3, 689, gateCellWidth - 8, 32);
    const auto lowerWidth = contentWidth - 88;
    const auto lowerCellWidth = lowerWidth / 4;
    gateGrid.setBounds (112, 730, lowerWidth, 122);
    swingLabel.setBounds (24, 776, 72, 20);
    gateSwing.setBounds (24, 798, 72, 150);
    const std::array<juce::Component*, 4> controls {
        &gateAttack, &gateRelease, &gateTension, &outputGain
    };
    for (size_t i = 0; i < controls.size(); ++i)
    {
        const auto x = 112 + static_cast<int> (i) * lowerCellWidth;
        const auto controlWidth = lowerCellWidth - 8;
        gateControlLabels[i].setBounds (x, 860, controlWidth, 14);
        controls[i]->setBounds (x, 874, controlWidth, 32);
    }
    gateFilter.setBounds (112, 914, 130, 28);
    gateStereo.setBounds (248, 914, 150, 28);
    presetDescription.setBounds (408, 910, contentWidth - 384, 50);
}

void MKNoiseRhythmMachineEditor::GateGrid::paint (juce::Graphics& g)
{
    auto area = getLocalBounds().toFloat();
    g.setColour (juce::Colour (0xff121b27));
    g.fillRoundedRectangle (area, 6.0f);
    const auto steps = processor.getGateStepCount();
    const auto plot = plotBounds();
    std::array<float, 32> levels {}, offsets {};
    for (int step = 0; step < steps; ++step)
    {
        levels[static_cast<size_t> (step)] = processor.getGateStep (step);
        offsets[static_cast<size_t> (step)] = processor.getGatePointOffset (step);
        g.setColour (juce::Colour (0xff27394b));
        const auto x = plot.getX() + plot.getWidth() * static_cast<float> (step) / (steps - 1);
        g.drawVerticalLine (static_cast<int> (x), plot.getY(), plot.getBottom());
    }
    const auto shape = juce::roundToInt (processor.parameters.getRawParameterValue ("gateShape")->load());
    const auto tension = processor.parameters.getRawParameterValue ("gateTension")->load();
    juce::Path curve;
    for (int sample = 0; sample <= 400; ++sample)
    {
        const auto phase = static_cast<float> (sample) / 400.0f;
        const auto level = MKNoiseRhythmMachineProcessor::evaluateGateCurve (
            previewPattern, levels, offsets, steps, shape, phase, tension);
        const auto x = plot.getX() + phase * plot.getWidth();
        const auto y = plot.getBottom() - level * plot.getHeight();
        if (sample == 0) curve.startNewSubPath (x, y);
        else curve.lineTo (x, y);
    }
    g.setColour (juce::Colour (0xfff18b4c));
    g.strokePath (curve, juce::PathStrokeType (2.0f));
    for (int point = 0; point < steps; ++point)
    {
        const auto offset = point == 0 || point == steps - 1 ? 0.0f : offsets[static_cast<size_t> (point)];
        const auto x = plot.getX() + (point + offset) / (steps - 1) * plot.getWidth();
        const auto y = plot.getBottom() - levels[static_cast<size_t> (point)] * plot.getHeight();
        g.setColour (juce::Colour (point == selectedPoint ? 0xfff7faff : 0xff35c4ce));
        g.fillEllipse (x - 5.0f, y - 5.0f, 10.0f, 10.0f);
    }
    g.setColour (juce::Colour (0xff35c4ce));
    const auto seek = plot.getX() + processor.getGatePlaybackPhase() * plot.getWidth();
    g.drawVerticalLine (static_cast<int> (seek), plot.getY(), plot.getBottom());
    g.setColour (juce::Colour (0xff60778d));
    g.drawRoundedRectangle (area, 6.0f, 1.0f);
}

void MKNoiseRhythmMachineEditor::GateGrid::setStepFromPosition (juce::Point<float> position)
{
    if (selectedPoint < 0)
        return;
    const auto steps = processor.getGateStepCount();
    const auto plot = plotBounds();
    const auto phase = juce::jlimit (0.0f, 1.0f, (position.x - plot.getX()) / plot.getWidth());
    const auto value = juce::jlimit (0.0f, 1.0f, (plot.getBottom() - position.y) / plot.getHeight());
    processor.setGateStep (selectedPoint, value);
    if (selectedPoint > 0 && selectedPoint < steps - 1)
    {
        auto* parameter = processor.parameters.getParameter (processor.gatePositionParameterID (selectedPoint));
        const auto freePositions = processor.parameters.getRawParameterValue ("gateUsePositions")->load() > 0.5f;
        const auto neighbour = [this, steps] (int point) {
            return (point + processor.getGatePointOffset (point)) / (steps - 1);
        };
        const auto lower = neighbour (selectedPoint - 1) + 0.000001f;
        const auto upper = juce::jmax (lower, neighbour (selectedPoint + 1) - 0.000001f);
        const auto time = freePositions
            ? juce::jlimit (lower, upper, phase)
            : juce::jlimit (-0.49f, 0.49f, phase * (steps - 1) - selectedPoint);
        parameter->setValueNotifyingHost (parameter->convertTo0to1 (time));
    }
    repaint();
}

void MKNoiseRhythmMachineEditor::GateGrid::mouseDown (const juce::MouseEvent& event)
{
    const auto steps = processor.getGateStepCount();
    const auto plot = plotBounds();
    auto closest = std::numeric_limits<float>::max();
    for (int point = 0; point < steps; ++point)
    {
        const auto offset = point == 0 || point == steps - 1 ? 0.0f
            : processor.getGatePointOffset (point);
        const juce::Point<float> pos {
            plot.getX() + (point + offset) / (steps - 1) * plot.getWidth(),
            plot.getBottom() - processor.getGateStep (point) * plot.getHeight()
        };
        const auto distance = pos.getDistanceSquaredFrom (event.position);
        if (distance < closest)
        {
            closest = distance;
            selectedPoint = point;
        }
    }
    processor.parameters.getParameter ("gateStep" + juce::String (selectedPoint + 1).paddedLeft ('0', 2))->beginChangeGesture();
    processor.parameters.getParameter (processor.gatePositionParameterID (selectedPoint))->beginChangeGesture();
    setStepFromPosition (event.position);
}

void MKNoiseRhythmMachineEditor::GateGrid::mouseDrag (const juce::MouseEvent& event)
{
    setStepFromPosition (event.position);
}

void MKNoiseRhythmMachineEditor::GateGrid::mouseUp (const juce::MouseEvent&)
{
    if (selectedPoint < 0)
        return;
    processor.parameters.getParameter ("gateStep" + juce::String (selectedPoint + 1).paddedLeft ('0', 2))->endChangeGesture();
    processor.parameters.getParameter (processor.gatePositionParameterID (selectedPoint))->endChangeGesture();
    selectedPoint = -1;
}

juce::Rectangle<float> MKNoiseRhythmMachineEditor::GateGrid::plotBounds() const
{
    return getLocalBounds().toFloat().reduced (12.0f);
}

void MKNoiseRhythmMachineEditor::timerCallback()
{
    gateGrid.repaint();
    const auto preset = juce::roundToInt (processor.parameters.getRawParameterValue ("gatePreset")->load());
    // A user selection is delivered asynchronously; do not overwrite it with the unchanged saved index.
    if (preset != lastSyncedPreset)
    {
        lastSyncedPreset = preset;
        gatePreset.setSelectedItemIndex (preset, juce::dontSendNotification);
        presetDescription.setText (processor.getGatePresetDescription(), juce::dontSendNotification);
    }
}
