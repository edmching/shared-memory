#include "maze_runner_common.h"

#include <cpen333/process/shared_memory.h>
#include <cpen333/process/mutex.h>
#include <cstring>
#include <chrono>
#include <thread>

class MazeRunner {

  cpen333::process::shared_object<SharedData> memory_;
  cpen333::process::mutex mutex_;

  // local copy of maze
  MazeInfo minfo_;

  // runner info
  size_t idx_;   // runner index
  int loc_[2];   // current location

 public:

  MazeRunner() : memory_(MAZE_MEMORY_NAME), mutex_(MAZE_MUTEX_NAME),
                 minfo_(), idx_(0), loc_() {

    // copy maze contents
    minfo_ = memory_->minfo;

    {
      // protect access of number of runners
      std::lock_guard<decltype(mutex_)> lock(mutex_);
      idx_ = memory_->rinfo.nrunners;
      memory_->rinfo.nrunners++;
    }

    // get current location
    loc_[COL_IDX] = memory_->rinfo.rloc[idx_][COL_IDX];
    loc_[ROW_IDX] = memory_->rinfo.rloc[idx_][ROW_IDX];

  }

  /**
   * Solves the maze, taking time between each step so we can actually see progress in the UI
   * @return 1 for success, 0 for failure, -1 to quit
   */
  int go() {
	  // current location
	  int c = loc_[COL_IDX];
	  int r = loc_[ROW_IDX];

	  std::this_thread::sleep_for(std::chrono::milliseconds(100));

	  //==========================================================
	  // TODO: NAVIGATE MAZE
	  //==========================================================
	  int i;
	  int j;
	  while (1) {
		  for (i = -1; i < 2; i++)
		  {
			  for (j = -1; j < 2; j++) {

				  if (memory_->quit == true)
					  return -1;
				  if (minfo_.maze[c + i][r + j] == EMPTY_CHAR)
				  {
					  memory_->rinfo.rloc[idx_][COL_IDX] = c + i;
					  memory_->rinfo.rloc[idx_][ROW_IDX] = r + j;
					  std::cout << "found new spot! moved:" << i << "," << j << std::endl;
					  break;
				  }
				  if (minfo_.maze[c + i][r + j] == EXIT_CHAR)
				  {
					  memory_->rinfo.rloc[idx_][COL_IDX] = c + i;
					  memory_->rinfo.rloc[idx_][COL_IDX] = r + j;
					  return 1;
				  }

			  }
			  if (minfo_.maze[c + i][r + j] == EMPTY_CHAR)
				  break;
		  }
	 }
  
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
	

    // failed to find exit
    return 0;
  }

};

int main() {

  MazeRunner runner;
  runner.go();

  return 0;
}