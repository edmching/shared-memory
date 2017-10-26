#include "maze_runner_common.h"

#include <cpen333/process/shared_memory.h>
#include <cpen333/process/mutex.h>
#include <cstring>
#include <chrono>
#include <thread>
#include <random>

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
    loc_[COL_IDX] = memory_->rinfo.rloc[idx_][COL_IDX];//since idx is already set from before; so we dont need a mutex
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
	  int new_col;
	  int new_row;

	  std::this_thread::sleep_for(std::chrono::milliseconds(100));

	  //==========================================================
	  // TODO: NAVIGATE MAZE
	  //==========================================================
	
	  std::default_random_engine rnd(
		   (int)std::chrono::system_clock::now().time_since_epoch().count());
	  std::uniform_int_distribution<int> rdist(-1, 1);
	  std::uniform_int_distribution<int> cdist(-1, 1);

	  for(int i = 0; i<1000; ++i) {
		  new_col = c + cdist(rnd);
		  new_row = r + rdist(rnd);
		  if (minfo_.maze[new_col][new_row] == EMPTY_CHAR) {
			  memory_->rinfo.rloc[idx_][COL_IDX] = new_col;
			  memory_->rinfo.rloc[idx_][ROW_IDX] = new_row;
			  std::this_thread::sleep_for(std::chrono::milliseconds(100));
		  }
		  if (memory_->quit == true)
			  return -1;
		  if (minfo_.maze[new_col][new_row] == EXIT_CHAR)
			  return 1;
	  }

    // failed to find exit
    return 0;
  }

};

int main() {

  cpen333::process::shared_object<SharedData> memory_(MAZE_MEMORY_NAME);
  cpen333::process::mutex mutex_(MAZE_MUTEX_NAME);

  int magic;
  {
	std::lock_guard<decltype(mutex_)> mutex(mutex_);
	magic = memory_->magic;
  }

  //check for initialization
  if (magic == 604123)
  {
	 MazeRunner runner;
	 runner.go();
  }
  else {
	  std::cout << "Error Maze_runner_main not initialized" << std::endl;
  }
  return 0;
}