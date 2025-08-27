cmake_minimum_required(VERSION 3.15)
project(MasterSuite VERSION 0.1)
# Assumes JUCE is available as ../JUCE (clone JUCE next to this project or adjust path)
add_subdirectory(../JUCE JUCE)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

juce_add_plugin(MasterSuite
    COMPANY_NAME "MyCompany"
    BUNDLE_ID    com.mycompany.mastersuite
    IS_SYNTH     FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD TRUE
    PLUGIN_MANUFACTURER_CODE MCmp
    PLUGIN_CODE MSP1
    FORMATS VST3
    PRODUCT_NAME "MasterSuite")

file(GLOB_RECURSE SRC CONFIGURE_DEPENDS Source/*)
juce_generate_juce_header(MasterSuite)
target_sources(MasterSuite PRIVATE ${SRC})

target_link_libraries(MasterSuite PRIVATE
    juce::juce_audio_utils
    juce::juce_audio_processors
    juce::juce_dsp
    juce::juce_gui_extra)
