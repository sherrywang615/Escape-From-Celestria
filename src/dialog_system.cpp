#include "dialog_system.hpp"
#include "tiny_ecs_registry.hpp"

#include <fstream>

std::vector<std::pair<int, std::string>> DialogSystem::parseDialog(std::fstream& file) {
	std::string line;
	int speaker = -1;
	std::vector<std::pair<int, std::string>> result;

	while (getline(file, line))
	{
		if (line.empty())
		{
			break;
		}
		else if (line[0] == '@')
		{
			if (line[1] == 'J')
			{
				speaker = 0;
			}
			else
			{
				speaker = line[1] - '0';
			}
		}
		else
		{
			std::pair<int, std::string> sentence;
			sentence.first = speaker;
			sentence.second = line;
			result.push_back(sentence);
		}
	}

	return result;
}

DialogSystem::DialogSystem()
{

}

DialogSystem::~DialogSystem()
{
    cleanDialog();
}

void DialogSystem::cleanDialog()
{
    dialog_data.clear();
    registry.speech.clear();
}

void DialogSystem::initializeDialog(std::string dialog_file)
{
	cleanDialog();
	std::fstream file;
	file.open(dialog_file);

	if (file.is_open())
	{
		std::string line;
		while (getline(file, line))
		{
			if (line[0] == ':')
			{
				unsigned int index = line[1] - '0';
				dialog_data[index] = parseDialog(file);
			}
		}
	}
	else
	{
		printf("Cannot load map. Check file path!\n");
	}
	file.close();
}

void DialogSystem::createSpeechPoint(unsigned int speech_point)
{
	std::vector<std::pair<int, std::string>> dialog = dialog_data[speech_point];
	for (std::pair<int, std::string> sentence : dialog)
	{
		createSpeech(findEntityById(sentence.first), sentence.second, 3000.0f);
	}
}

void DialogSystem::createSpeech(Entity speaker, std::string text, float time) {
	if (registry.speech.has(registry.players.entities[0]))
	{
		Speech& dialog = registry.speech.get(registry.players.entities[0]);
		std::pair<Entity, std::string> sentence;
		sentence.first = speaker;
		sentence.second = text;
		dialog.texts.push(sentence);
		dialog.timer.push(time);
		dialog.counter_ms = time;
	}
	else
	{
		Speech& dialog = registry.speech.emplace(registry.players.entities[0]);
		std::pair<Entity, std::string> sentence;
		sentence.first = speaker;
		sentence.second = text;
		dialog.texts.push(sentence);
		dialog.timer.push(time);
		dialog.counter_ms = time;
	}
}

Entity DialogSystem::findEntityById(int id)
{
	if (id == 0)
	{
		return registry.players.entities[0];
	}

	for (Entity entity : registry.nonPlayerCharacter.entities)
	{
		NonPlayerCharacter& npc = registry.nonPlayerCharacter.get(entity);
		if (npc.id == id)
		{
			return entity;
		}
	}

	printf("ERROR: cannot find NPC %i\n", id);
	return Entity();
}