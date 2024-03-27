#pragma once
#include <vector>

#include "tiny_ecs.hpp"
#include "components.hpp"

class ECSRegistry
{
	// Callbacks to remove a particular or all entities in the system
	std::vector<ContainerInterface *> registry_list;

public:
	// Manually created list of all components this game has
	// TODO: A1 add a LightUp component
	ComponentContainer<DeathTimer> deathTimers;
	ComponentContainer<Motion> motions;
	ComponentContainer<Collision> collisions;
	ComponentContainer<Player> players;
	ComponentContainer<Mesh *> meshPtrs;
	ComponentContainer<RenderRequest> renderRequests;
	ComponentContainer<ScreenState> screenStates;
	ComponentContainer<Eatable> eatables;
	ComponentContainer<Deadly> deadlys;
	ComponentContainer<NormalZombie> zombies;
	ComponentContainer<Platform> platforms;
	ComponentContainer<DebugComponent> debugComponents;
	ComponentContainer<vec3> colors;
	ComponentContainer<Sliding> slidings;
	ComponentContainer<Gravity> gravities;
	ComponentContainer<ColorChange> colorChanges;
	ComponentContainer<DeductHpTimer> deductHpTimers;
	ComponentContainer<Door> doors;
	ComponentContainer<Key> keys;
	ComponentContainer<Bullet> bullets;
	ComponentContainer<Food> foods;
	ComponentContainer<Character> characters;
	ComponentContainer<Heart> hearts;
	ComponentContainer<Cabinet> cabinets;
	ComponentContainer<SmallBullet> smallBullets;
	ComponentContainer<ShootBullet> shootBullets;
	ComponentContainer<Text> texts;
	ComponentContainer<MenuElement> menus;
	ComponentContainer<NonPlayerCharacter> nonPlayerCharacter;
	ComponentContainer<Speech> speech;
	ComponentContainer<Timer> timer;
	ComponentContainer<SpeechPoint> speechPoint;
	ComponentContainer<Gold> golds;
	ComponentContainer<Fireball> fireballs;
	ComponentContainer<InvincibleTimer> invincibleTimers;

	// constructor that adds all containers for looping over them
	// IMPORTANT: Don't forget to add any newly added containers!
	ECSRegistry()
	{
		// TODO: A1 add a LightUp component
		registry_list.push_back(&deathTimers);
		registry_list.push_back(&motions);
		registry_list.push_back(&collisions);
		registry_list.push_back(&players);
		registry_list.push_back(&meshPtrs);
		registry_list.push_back(&renderRequests);
		registry_list.push_back(&screenStates);
		registry_list.push_back(&eatables);
		registry_list.push_back(&deadlys);
		registry_list.push_back(&zombies);
		registry_list.push_back(&platforms);
		registry_list.push_back(&debugComponents);
		registry_list.push_back(&colors);
		registry_list.push_back(&slidings);
		registry_list.push_back(&gravities);
		registry_list.push_back(&colorChanges);
		registry_list.push_back(&deductHpTimers);
		registry_list.push_back(&doors);
		registry_list.push_back(&keys);
		registry_list.push_back(&bullets);
		registry_list.push_back(&foods);
		registry_list.push_back(&characters);
		registry_list.push_back(&hearts);
		registry_list.push_back(&cabinets);
		registry_list.push_back(&smallBullets);
		registry_list.push_back(&shootBullets);
		registry_list.push_back(&texts);
		registry_list.push_back(&menus);
		registry_list.push_back(&nonPlayerCharacter);
		registry_list.push_back(&speech);
		registry_list.push_back(&timer);
		registry_list.push_back(&speechPoint);
		registry_list.push_back(&golds);
		registry_list.push_back(&fireballs);
		registry_list.push_back(&invincibleTimers);
	}

	void clear_all_components()
	{
		for (ContainerInterface *reg : registry_list)
			reg->clear();
	}

	void list_all_components()
	{
		printf("Debug info on all registry entries:\n");
		for (ContainerInterface *reg : registry_list)
			if (reg->size() > 0)
				printf("%4d components of type %s\n", (int)reg->size(), typeid(*reg).name());
	}

	void list_all_components_of(Entity e)
	{
		printf("Debug info on components of entity %u:\n", (unsigned int)e);
		for (ContainerInterface *reg : registry_list)
			if (reg->has(e))
				printf("type %s\n", typeid(*reg).name());
	}

	void remove_all_components_of(Entity e)
	{
		for (ContainerInterface *reg : registry_list)
			reg->remove(e);
	}
};

extern ECSRegistry registry;