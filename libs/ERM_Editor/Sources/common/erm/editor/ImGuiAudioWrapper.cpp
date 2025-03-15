#include "erm/editor/ImGuiAudioWrapper.h"

#include <erm/audio/AudioManager.h>
#include <erm/audio/Sound.h>

#include <erm/engine/Engine.h>

#include <erm/utils/Utils.h>

#include <imgui.h>

namespace ImGui {

void ShowAudioDebugWindow(erm::Engine& engine, bool& open)
{
	if (ImGui::Begin("Audio", &open, ImGuiWindowFlags_NoCollapse))
	{
		erm::AudioManager& audioManager = engine.getAudioManager();
		const auto& sounds = engine.getFileLocator().getSounds();
		const auto& drivers = audioManager.getDrivers();
		const auto& currentDriver = drivers.at(audioManager.getDriver());

		bool playInBackground = audioManager.shouldPlayInBackground();
		if (ImGui::Checkbox("Play in background", &playInBackground))
			audioManager.setPlayInBackground(playInBackground);

		if (ImGui::Button("R"))
			audioManager.updateDrivers();

		ImGui::SameLine();

		if (ImGui::BeginCombo("Drivers", currentDriver.mName.data()))
		{
			for (const auto& [id, driver] : drivers)
			{
				bool selected = id == audioManager.getDriver();
				if (ImGui::Selectable(driver.mName.data(), &selected))
				{
					audioManager.setDriver(id);
				}
			}

			ImGui::EndCombo();
		}

		ImGui::Separator();

		bool play = ImGui::Button("+");

		ImGui::SameLine();

		static std::string selectedValue = "";
		if (ImGui::BeginCombo("Path", selectedValue.data()))
		{
			for (auto& sound : sounds)
			{
				bool isSelected = selectedValue == sound;
				if (ImGui::Selectable(sound.data(), &isSelected))
				{
					selectedValue = sound;
				}
			}

			ImGui::EndCombo();
		}

		erm::Sound* sound = selectedValue.empty() ? nullptr : audioManager.getSound(selectedValue.data(), true);
		unsigned int duration = 0;

		if (sound)
		{
			duration = sound->getLenght() / 1000;
		}

		ImGui::Text("Duration: %s", erm::utils::formatTime(duration).data());

		if (sound && play)
		{
			if (auto repro = audioManager.playSound(*sound))
				repro->pause();
		}

		ImGui::Separator();

		const auto& reproductions = audioManager.getReproductions();

		for (int i = 0; i < static_cast<int>(reproductions.size()); ++i)
		{
			const auto& repro = reproductions[i];
			if (repro.isPlaying())
			{
				ImGui::PushID(i);

				ImGui::BeginChild("ChildL", ImVec2(ImGui::GetWindowContentRegionMax().x * 0.5f, 150), true);
				bool loop = repro.isLooping();
				if (ImGui::Checkbox("Loop", &loop))
				{
					repro.setLoopCount(loop ? erm::LoopCount::FOREVER : erm::LoopCount::NO_LOOP);
					repro.setMode(loop ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF);
				}

				ImGui::SameLine();

				bool muted = repro.isMuted();
				if (ImGui::Checkbox("Mute", &muted))
				{
					repro.setMuted(muted);
				}

				if (repro.isPaused())
				{
					if (ImGui::Button(">"))
						repro.resume();
				}
				else
				{
					if (ImGui::Button("||"))
						repro.pause();
				}

				ImGui::SameLine();

				if (ImGui::Button("O"))
				{
					repro.stop();
					ImGui::EndChild();
					ImGui::PopID();
					continue;
				}

				ImGui::SameLine();
				ImGui::Text("%s", repro.getCurrentSoundName().data());

				static std::array<bool, erm::AudioManager::MAX_CHANNELS> sAutomaticallyPaused {false};
				const unsigned int lenght = static_cast<unsigned int>(repro.getDuration());
				float position = repro.getPosition();
				const float min = 0.0f;
				const float max = 1.0f;

				std::string label = erm::utils::formatTime(static_cast<unsigned int>(static_cast<float>(lenght) * position));
				label += "/";
				label += erm::utils::formatTime(lenght);

				if (ImGui::SliderScalar(label.data(), ImGuiDataType_Float, &position, &min, &max))
				{
					if (!repro.isPaused())
					{
						sAutomaticallyPaused[i] = true;
						repro.pause();
					}
					repro.setPosition(position);
				}
				else if (sAutomaticallyPaused[i] && repro.isPaused())
				{
					sAutomaticallyPaused[i] = false;
					repro.resume();
				}

				float pitch = repro.getPitch();
				if (ImGui::DragFloat("Pitch", &pitch, 0.05f, 0.0f, 2.0f))
					repro.setPitch(pitch);

				float frequency = repro.getFrequency();
				const bool setFrequency = ImGui::DragFloat(
					"Frequency",
					&frequency,
					100.0f,
					(repro.isStream() ? -50000.0f : 0.0f),
					50000.0f);

				bool invertFrequency = false;
				if (!repro.isStream())
				{
					ImGui::SameLine();
					invertFrequency = ImGui::Button("+/-");
				}

				if (setFrequency || invertFrequency)
					repro.setFrequency(frequency * (invertFrequency ? -1.0f : 1.0f));
				ImGui::EndChild();

				ImGui::SameLine();

				ImGui::BeginChild("ChildR", ImVec2(100, 150), true);
				float volume = repro.getVolume();
				ImGui::PushID("NO_SELECTABLE");
				if (ImGui::VSliderFloat("##", ImVec2(20, 125), &volume, -10.0f, 10.0f))
					repro.setVolume(volume);
				ImGui::PopID();
				ImGui::EndChild();

				ImGui::Separator();
				ImGui::PopID();
			}
		}
	}

	ImGui::End();
}

} // namespace ImGui
