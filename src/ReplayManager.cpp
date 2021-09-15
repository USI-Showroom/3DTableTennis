#include "ReplayManager.h"

//file reconstructed (aug 2021) since SOMEHOW missing from repo

ReplayManager ReplayManager::Instance(){
	static ReplayManager theInstance;
	return theInstance;
}

void ReplayManager::clear() {
	ReplayManager::points.clear();
}

std::vector<Point3d> ReplayManager::getPoints() {
	return ReplayManager::points;
}

void ReplayManager::add(Point3d position) {
	ReplayManager::points.push_back(position);
}
