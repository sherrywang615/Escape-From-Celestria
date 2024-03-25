#include "world_helper.hpp"

float offset = PLATFORM_HEIGHT / 2 + (ZOMBIE_BB_HEIGHT * 0.6) / 2 + 10;


void createVerticesForLevel1() {
	Vertex* first_floor_jump = graph.getVertex(910, (int)700 - offset);
	Vertex* first_mid_plat = graph.getVertex(940, (int)600 - offset);
	graph.addJumpEdge(first_floor_jump, first_mid_plat);
	Vertex* second_floor_land = graph.getVertex(820, (int)500 - offset);
	graph.addJumpEdge(first_mid_plat, second_floor_land);
	Vertex* second_floor_jump = graph.getVertex(140, (int)500 - offset);
	Vertex* second_mid_plat = graph.getVertex(90, (int)400 - offset);
	graph.addJumpEdge(second_floor_jump, second_mid_plat);
	Vertex* third_floor_land = graph.getVertex(220, (int)300 - offset);
	graph.addJumpEdge(second_mid_plat, third_floor_land);
	Vertex* third_mid_plat = graph.getVertex(100, (int)200 - offset);
	graph.addJumpEdge(third_floor_land, third_mid_plat);
	Vertex* fourth_floor_land = graph.getVertex(210, (int)120 - offset);
	graph.addJumpEdge(third_mid_plat, fourth_floor_land);
	Vertex* fourth_floor_second_land = graph.getVertex(800, (int)120 - offset);
	Vertex* third_floor_second_jump = graph.getVertex(820, (int)300 - offset);
	graph.addJumpEdge(third_floor_second_jump, fourth_floor_second_land);

}

void createVerticesForLevel2() {
	Vertex* first_floor_land = graph.getVertex(230, (int)710 - offset);
	Vertex* first_mid_plat = graph.getVertex(70, (int)620 - offset);
	graph.addJumpEdge(first_floor_land, first_mid_plat);
	Vertex* second_floor_land = graph.getVertex(180, (int)520 - offset);
	graph.addJumpEdge(first_mid_plat, second_floor_land);
	Vertex* second_floor_left_jump = graph.getVertex(330, (int)520 - offset);
	Vertex* second_floor_plat_right = graph.getVertex(550, (int)420 - offset);
	graph.addJumpEdge(second_floor_left_jump, second_floor_plat_right);
	Vertex* second_floor_plat_right_land = graph.getVertex(540, (int)420 - offset);
	Vertex* second_floor_right_jump = graph.getVertex(530, (int)520 - offset);
	graph.addJumpEdge(second_floor_right_jump, second_floor_plat_right_land);
	Vertex* second_plat_left = graph.getVertex(70, (int)400 - offset);
	graph.addJumpEdge(second_floor_land, second_plat_left);
	Vertex* third_floor_left = graph.getVertex(190, (int)310 - offset);
	graph.addJumpEdge(second_plat_left, third_floor_left);
	Vertex* third_floor_right = graph.getVertex(400, (int)310 - offset);
	graph.addJumpEdge(second_floor_plat_right_land, third_floor_right);
	Vertex* third_floor_rl = graph.getVertex(780, (int)310 - offset);
	graph.addJumpEdge(second_floor_plat_right_land, third_floor_rl);
	Vertex* third_plat = graph.getVertex(70, (int)210 - offset);
	graph.addJumpEdge(third_floor_left, third_plat);
	Vertex* fourth_plat = graph.getVertex(190, (int)130 - offset);
	graph.addJumpEdge(third_plat, fourth_plat);
}  

void createVerticesForLevel3() {
	Vertex* first_floor_jump = graph.getVertex(350, (int)700 - offset);
	Vertex* second_floor_land = graph.getVertex(410, (int)580 - offset);
	graph.addJumpEdge(first_floor_jump, second_floor_land);
	Vertex* second_floor_jump_mid = graph.getVertex(610, (int)580 - offset);
	Vertex* third_floor_land_left = graph.getVertex(680, (int)460 - offset);
	graph.addJumpEdge(second_floor_jump_mid, third_floor_land_left);
	Vertex* second_floor_jump_right = graph.getVertex(910, (int)580 - offset);
	Vertex* third_floor_land_right = graph.getVertex(860, (int)460 - offset);
	graph.addJumpEdge(second_floor_jump_right, third_floor_land_right);
	Vertex* second_plat_right = graph.getVertex(300, (int)500 - offset);
	graph.addJumpEdge(second_floor_land, second_plat_right);
	Vertex* second_plat_left = graph.getVertex(160, (int)500 - offset);
	Vertex* first_floor_land = graph.getVertex(110, (int)700 - offset);
	graph.addEdge(second_plat_left, first_floor_land, ACTION::WALK);
	Vertex* third_plat = graph.getVertex(110, (int)400 - offset);
	graph.addJumpEdge(second_plat_left, third_plat);
	Vertex* third_floor_left = graph.getVertex(220, (int)320 - offset);
	graph.addJumpEdge(third_plat, third_floor_left);
	Vertex* door = graph.getVertex(200, (int)190 - offset);
	graph.addJumpEdge(third_floor_left, door);
	Vertex* fourth_floor_left = graph.getVertex(450, (int)320 - offset);
	Vertex* fifth_floor_left = graph.getVertex(520, (int)160 - offset);
	graph.addJumpEdge(fourth_floor_left, fifth_floor_left);
	Vertex* fifth_floor_right = graph.getVertex(770, (int)160 - offset);
	Vertex* fourth_floor_right = graph.getVertex(820, (int)320 - offset);
	graph.addJumpEdge(fourth_floor_right, fifth_floor_left);
	Vertex* fifth_floor_rl = graph.getVertex(910, (int)210 - offset);
	graph.addJumpEdge(fourth_floor_right, fifth_floor_rl);
}

void createVerticesForLevel4() {
	Vertex* first_floor_jump = graph.getVertex(170, (int)700 - offset);
	Vertex* second_floor_left = graph.getVertex(250, (int)570 - offset);
	graph.addJumpEdge(first_floor_jump, second_floor_left);
	Vertex* second_floor_right = graph.getVertex(800, (int)570 - offset);
	Vertex* first_floor_right = graph.getVertex(870, (int)700 - offset);
	graph.addJumpEdge(first_floor_right, second_floor_right);
	Vertex* second_plat_left = graph.getVertex(140, (int)480 - offset);
	graph.addJumpEdge(second_floor_left, second_plat_left);
	Vertex* second_plat_right = graph.getVertex(900, (int)480 - offset);
	graph.addJumpEdge(second_floor_right, second_plat_right);
	Vertex* third_floor_right = graph.getVertex(790, (int)380 - offset);
	graph.addJumpEdge(second_plat_right, third_floor_right);
	Vertex* third_floor_right_left = graph.getVertex(660, (int)380 - offset);
	Vertex* fourth_floor_right = graph.getVertex(600, (int)300 - offset);
	graph.addJumpEdge(third_floor_right_left, fourth_floor_right);
	Vertex* third_floor_left_right = graph.getVertex(390, (int)380 - offset);
	Vertex* fourth_floor_left = graph.getVertex(440, (int)300 - offset);
	graph.addJumpEdge(third_floor_left_right, fourth_floor_left);
	Vertex* third_floor_left_left = graph.getVertex(240, (int)380 - offset);
	Vertex* fifth_floor_left = graph.getVertex(140, (int)270 - offset);
	graph.addJumpEdge(third_floor_left_left, fifth_floor_left);
	Vertex* fifth_floor_right = graph.getVertex(890, (int)250 - offset);
	graph.addJumpEdge(third_floor_right, fifth_floor_right);
	Vertex* sixth_floor_left = graph.getVertex(250, (int)170 - offset);
	graph.addJumpEdge(fifth_floor_left, sixth_floor_left);
	Vertex* sixth_floor_right = graph.getVertex(760, (int)170 - offset);
	graph.addJumpEdge(fifth_floor_right, sixth_floor_right);

}

void createGraph(int level) {
	//graph = Graph();
	if (level == 1) {
		createVerticesForLevel1();
	}
	else if (level == 2) {
		createVerticesForLevel2();
	}
	else if (level == 3) {
		createVerticesForLevel3();
	}
	else if (level == 4) {
		createVerticesForLevel4();
	}
}