// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#include <string>  // for basic_string, string, allocator
#include <vector>  // for vector
#include <fstream>
#include <cmath>
#include <thread>  // for sleep_for

#include "ftxui/component/component.hpp"  // for Button, Horizontal, Renderer, Dropdown, Horizontal, Vertical
#include "ftxui/component/component_base.hpp"      // for ComponentBase
#include "ftxui/component/screen_interactive.hpp"  // for ScreenInteractive
#include "ftxui/component/captured_mouse.hpp"      // for ftxui

#ifndef M_PI
  #define M_PI 3.14159265358979323846
#endif

using namespace ftxui;
using namespace std;

struct WAVHeader {
  char riff[4] = {'R', 'I', 'F', 'F'};
  uint32_t chunkSize;
  char wave[4] = {'W', 'A', 'V', 'E'};
  char fmt[4] = {'f', 'm', 't', ' '};
  uint32_t subchunk1Size = 16;
  uint16_t audioFormat = 1;
  uint16_t numChannels = 2;
  uint32_t sampleRate = 48000;
  uint32_t byteRate;
  uint16_t blockAlign;
  uint16_t bitsPerSample = 16;
  char data[4] = {'d', 'a', 't', 'a'};
  uint32_t subchunk2Size;
};

struct audio_data {
  uint16_t sample_rate;
  uint16_t amplitude;
  uint16_t channel;
  uint16_t frequence;
  uint16_t bitrate;
};

// button style
ButtonOption Style() {
  auto option = ButtonOption::Animated();
  option.transform = [](const EntryState& s) {
    auto element = text(s.label);
    if (s.focused) {
      element |= bold;
    }
    return element | borderEmpty;
  };

  return option;
}

template<typename T>
bool generate_audio(
    audio_data &data,
    std::string filename,
    int duration
  ) {
  
  WAVHeader header;
  
  header.subchunk1Size = 16;
  header.audioFormat = 1;
  header.numChannels = data.channel;
  header.sampleRate = data.sample_rate;
  header.bitsPerSample = data.bitrate;

  uint32_t dataSize = data.sample_rate * duration;

  std::ofstream file_pcm((filename + ".pcm").c_str(), std::ios::binary);
  std::ofstream file_wav((filename + ".wav").c_str(), std::ios::binary);

  if (!file_pcm || !file_wav) {
    return false;
  }

  // Write HEADER wav file into a media file
  file_wav.write(reinterpret_cast<const char*>(&header),  sizeof(header));

  for (uint32_t i = 0; i < dataSize; ++i) {
    double t = static_cast<double>(i) / data.sample_rate;
    for (uint16_t channel = 0; channel < data.channel; ++channel) {
      uint16_t D = (channel%2);
      double sample  = data.amplitude * std::sin(2.0*M_PI*(data.frequence)*t + D*M_PI);
      T sample_int  = static_cast<T>(sample);
      file_pcm.write(reinterpret_cast<const char*>(&sample_int),  sizeof(sample_int));
      file_wav.write(reinterpret_cast<const char*>(&sample_int),  sizeof(sample_int));
    }
  }

  file_pcm.close();
  file_wav.close();

  return true;
}

int main() {
  int amplitude = 1000;
  int frequency = 1000;
  std::string res;

  audio_data data;
  std::string str_duration = "0";
  std::string str_filename = "no_name";
  std::string str_amplitude = std::to_string(amplitude);
  std::string str_frequency = std::to_string(frequency);

  int selected_id_sample_rate    = 0;
  int selected_id_channel        = 0;
  int selected_id_bits_persample = 0;

  std::vector<std::string> list_sample_rate    = { "48000", "44100", "16000", "8000" };
  std::vector<std::string> list_channel        = { "1", "2", "3", "4", "5", "6", "7", "8" };
  std::vector<std::string> list_bits_persample = { "16", "32" };

  int row = 0;
  Component com_sample_rate    = Dropdown(&list_sample_rate, &selected_id_sample_rate) | flex;
  Component com_amplitude      = Container::Horizontal({
    (Component)Input(&str_amplitude, "interger") | ftxui::vcenter | flex,
    (Component)Button("-100", [&] {
      if(amplitude > 0) amplitude -= 100;
      str_amplitude = std::to_string(amplitude);
      res = "";
    }, Style()),
    (Component)Button("+100", [&] {
      if(amplitude < 20000) amplitude += 100;
      str_amplitude = std::to_string(amplitude);
      res = "";
    }, Style()),
  }, &row) | flex;
  Component com_channel        = Dropdown(&list_channel, &selected_id_channel) | flex;
  Component com_frequency      = Container::Horizontal({
    (Component)Input(&str_frequency, "float(0.00)") | ftxui::vcenter | flex,
    (Component)Button("-100", [&] {
      if(frequency > 0) frequency -= 100;
      str_frequency = std::to_string(frequency);
      res = "";
    }, Style()),
    (Component)Button("+100", [&] {
      if(frequency < 20000) frequency += 100;
      str_frequency = std::to_string(frequency);
      res = "";
    }, Style()),
  }, &row) | flex;
  Component com_bits_persample = Dropdown(&list_bits_persample, &selected_id_bits_persample) | flex;
  Component com_txt_duration   = Input(&(str_duration), "seconds");
  Component com_txt_filename   = Input(&str_filename, "filename");
  Component com_btn_generate   = Button("Start generate",
    [&] {
      data.frequence   = stoi(str_frequency);
      data.amplitude   = stoi(str_amplitude);
      data.channel     = stoi(list_channel[selected_id_channel]);
      data.sample_rate = stoi(list_sample_rate[selected_id_sample_rate]);
      data.bitrate     = stoi(list_bits_persample[selected_id_bits_persample]);
      bool result = false;
      if(16 == data.bitrate) {
        result = generate_audio<int16_t>(data, str_filename, stoi(str_duration,0,10));
      } else if (32 == data.bitrate) {
        result = generate_audio<int32_t>(data, str_filename, stoi(str_duration,0,10));
      }
      if(result) {
        res = "[DONE] "+ str_filename + " : " 
              + "[ f:" + str_frequency
              + " | amp: " + str_amplitude
              + " | ch: " + list_channel[selected_id_channel]
              + " | sr: " + list_sample_rate[selected_id_sample_rate]
              + " | br: " + list_bits_persample[selected_id_bits_persample]
              + " ] (duration: " + str_duration + "s )";
            } else {
        res = "[FALSE] Generate file " + str_filename + " false.";
      }
    },
    Style());

  Component component = Container::Vertical({
    com_sample_rate,
    com_amplitude,
    com_channel,
    com_frequency,
    com_bits_persample,
    com_txt_duration,
    com_txt_filename,
    com_btn_generate,
  });

  auto renderer = Renderer(component, [&] {
    return vbox({
      hbox(text("Sample Rate     : ") | center, com_sample_rate->Render()),
      hbox(text("Amplitude       : ") | center, com_amplitude->Render()),
      hbox(text("Channel         : ") | center, com_channel->Render()),
      hbox(text("Frequency       : ") | center, com_frequency->Render()),
      hbox(text("Bits per sample : ") | center, com_bits_persample->Render()),
      hbox(text("Duration        : ") | center, com_txt_duration->Render()),
      hbox(text("File name       : ") | center, com_txt_filename->Render()),
      separator(),
      com_btn_generate->Render(),
      separator(),
      text(res),
    }) | border;
  });
 
  auto screen = ScreenInteractive::TerminalOutput();
  screen.Loop(renderer);
}