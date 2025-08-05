#pragma once

#include "Interfaces/Singleton.h"
#include "Snapshot/SceneSnapshot.h"
#include <deque>

#include "ECS/Systems/EntityManager.h"
#include "GameObject/ParentingManager.h"
#include <iostream>
#include <algorithm>

#ifdef min
#undef min
#endif

#ifdef max
#undef max
#endif

class TimelineManager : public Singleton<TimelineManager> {

	private:
		std::deque<SceneSnapshot*> timeline;
		int cursor = -1;
		bool isDirty = false;
	public: 
		const int LIMIT = 6;
		
	private:
		inline void PrintTimeline() {
			std::cout << "[ ";
			for (int i = 0; i < timeline.size(); i++) {
				std::cout << (cursor == i ? "X " : "0 ");
			}
			std::cout << "]" << std::endl;
		}

	public:
		inline void CreateSnapshot() {
			auto snapshot = new SceneSnapshot(EntityManager::get_all(), ParentingManager::get().GetAncestry());

			if (timeline.size() == LIMIT) {
				timeline.front()->ReleaseSnapshot();
				timeline.pop_front();
			}
				
			while (timeline.size() > cursor + 1) {
				timeline.back()->ReleaseSnapshot();
				timeline.pop_back();
			}

			timeline.push_back(snapshot);
			cursor++;
			isDirty = false;

			PrintTimeline();
		
			std::cout << "Created Snapshot!" << std::endl;
		}

		inline void Undo() {
			SetSnapshot(std::max(0, cursor - 1));
			PrintTimeline();
		}

		inline void Redo() {
		//	SetSnapshot(std::min(timeline.size()-1, cursor + 1));
			SetSnapshot(std::min(static_cast<int>(timeline.size() - 1), cursor + 1));
			PrintTimeline();
		}

		inline void SetDirty() { isDirty = true;}
		inline bool IsDirty() { return isDirty;}

	private:
		inline void SetSnapshot(int cursor_pos) {
			if (cursor != cursor_pos) {
				cursor = cursor_pos;
				ParentingManager::get().ReplaceHierarchy(timeline[cursor]->m_hierarchy);
				EntityManager::ReplaceHierarchy(timeline[cursor]->m_entities);
			}
		}

};