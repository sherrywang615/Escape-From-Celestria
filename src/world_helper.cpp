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

void createGraph(int level) {
	//graph = Graph();
	if (level == 1) {
		createVerticesForLevel1();
	}
	else if (level == 2) {
		createVerticesForLevel2();
	}
}