#include "erm/debug/ImGuiAudioWrapper.h"

#include "erm/audio/AudioManager.h"
#include "erm/audio/Sound.h"

#include "erm/engine/Engine.h"

#include "erm/utils/Utils.h"

#include <imgui.h>

namespace ImGui {

void ShowAudioDebugWindow(erm::Engine& engine, bool& open)
{
	if (ImGui::Begin("Audio", &open, ImGuiWindowFlags_NoCollapse))
	{
		erm::AudioManager& audioManager = engine.GetAudioManager();
		const auto& sounds = engine.GetFileLocator().GetSounds();
		const auto& drivers = audioManager.GetDrivers();
		const auto& currentDriver = drivers.at(audioManager.GetDriver());

		bool playInBackground = audioManager.ShouldPlayInBackground();
		if (ImGui::Checkbox("Play in background", &playInBackground))
			audioManager.SetPlayInBackground(playInBackground);

		if (ImGui::Button("R"))
			audioManager.UpdateDrivers();

		ImGui::SameLine();

		if (ImGui::BeginCombo("Drivers", currentDriver.mName.data()))
		{
			for (const auto& [id, driver] : drivers)
			{
				bool selected = id == audioManager.GetDriver();
				if (ImGui::Selectable(driver.mName.data(), &selected))
				{
					audioManager.SetDriver(id);
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

		erm::Sound* sound = selectedValue.empty() ? nullptr : audioManager.GetSound(selectedValue.data(), true);
		unsigned int duration = 0;

		if (sound)
		{
			duration = sound->GetLenght() / 1000;
		}

		ImGui::Text("Duration: %s", erm::Utils::FormatTime(duration).data());

		if (sound && play)
		{
			if (auto repro = audioManager.PlaySound(*sound))
				repro->Pause();
		}

		ImGui::Separator();

		const auto& reproductions = audioManager.GetReproductions();

		for (int i = 0; i < static_cast<int>(reproductions.size()); ++i)
		{
			const auto& repro = reproductions[i];
			if (repro.IsPlaying())
			{
				ImGui::PushID(i);

				ImGui::BeginChild("ChildL", ImVec2(ImGui::GetWindowContentRegionMax().x * 0.5f, 150), true);
				bool loop = repro.IsLooping();
				if (ImGui::Checkbox("Loop", &loop))
				{
					repro.SetLoopCount(loop ? erm::LoopCount::FOREVER : erm::LoopCount::NO_LOOP);
					repro.SetMode(loop ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF);
				}

				ImGui::SameLine();

				bool muted = repro.IsMuted();
				if (ImGui::Checkbox("Mute", &muted))
				{
					repro.SetMuted(muted);
				}

				if (repro.IsPaused())
				{
					if (ImGui::Button(">"))
						repro.Resume();
				}
				else
				{
					if (ImGui::Button("||"))
						repro.Pause();
				}

				ImGui::SameLine();

				if (ImGui::Button("O"))
				{
					repro.Stop();
					ImGui::EndChild();
					ImGui::PopID();
					continue;
				}

				ImGui::SameLine();
				ImGui::Text("%s", repro.GetCurrentSoundName());

				static std::array<bool, erm::AudioManager::MAX_CHANNELS> sAutomaticallyPaused {false};
				const unsigned int lenght = static_cast<unsigned int>(repro.GetDuration());
				float position = repro.GetPosition();
				const float min = 0.0f;
				const float max = 1.0f;

				std::string label = erm::Utils::FormatTime(static_cast<unsigned int>(static_cast<float>(lenght) * position));
				label += "/";
				label += erm::Utils::FormatTime(lenght);

				if (ImGui::SliderScalar(label.data(), ImGuiDataType_Float, &position, &min, &max))
				{
					if (!repro.IsPaused())
					{
						sAutomaticallyPaused[i] = true;
						repro.Pause();
					}
					repro.SetPosition(position);
				}
				else if (sAutomaticallyPaused[i] && repro.IsPaused())
				{
					sAutomaticallyPaused[i] = false;
					repro.Resume();
				}

				float pitch = repro.GetPitch();
				if (ImGui::DragFloat("Pitch", &pitch, 0.05f, 0.0f, 2.0f))
					repro.SetPitch(pitch);

				float frequency = repro.GetFrequency();
				const bool setFrequency = ImGui::DragFloat(
					"Frequency",
					&frequency,
					100.0f,
					(repro.IsStream() ? -50000.0f : 0.0f),
					50000.0f);

				bool invertFrequency = false;
				if (!repro.IsStream())
				{
					ImGui::SameLine();
					invertFrequency = ImGui::Button("+/-");
				}

				if (setFrequency || invertFrequency)
					repro.SetFrequency(frequency * (invertFrequency ? -1.0f : 1.0f));
				ImGui::EndChild();

				ImGui::SameLine();

				ImGui::BeginChild("ChildR", ImVec2(100, 150), true);
				float volume = repro.GetVolume();
				ImGui::PushID("NO_SELECTABLE");
				if (ImGui::VSliderFloat("", ImVec2(20, 125), &volume, -10.0f, 10.0f))
					repro.SetVolume(volume);
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
