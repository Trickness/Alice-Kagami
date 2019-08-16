#include "include/ThreadPool.hpp"
#include "include/Config.hpp"
#include <assert.h>

ThreadPool::ThreadPool(int init_size)
    :mMutex(),
    mCond(),
    mIsStarted(false)
    ,mInitThreadsSize(init_size)
    {
        this->Start();
}

ThreadPool::~ThreadPool(){
    if(mIsStarted){
        this->Stop();
    }
    this->mTasks.clear();
}

void ThreadPool::Start(){
    assert(mThreads.empty());
    mIsStarted = true;
    for(int i = 0; i < this->mInitThreadsSize; ++i){
        mThreads.push_back(new std::thread(std::bind(&ThreadPool::ThreadLoop, this)));
    }
}

void ThreadPool::Stop(){
    DEBUG_MSG("::Stop()");
    {
        std::unique_lock<std::mutex> lock(this->mMutex);
        mIsStarted = false;
        mCond.notify_all();
        DEBUG_MSG("::Stop() --> NotifyALl()");
    }
    for (ThreadsT::iterator it = this->mThreads.begin(); it != this->mThreads.end() ; ++it){
        (*it)->join();
        delete *it;
    }
    mThreads.clear();
}

void ThreadPool::ThreadLoop(){
    DEBUG_MSG(std::this_thread::get_id()  <<  "::ThreadLoop() Start");
    while(this->mIsStarted){
        TaskT task = this->Take();
        if(task){
            task();
        }
    }
    DEBUG_MSG(std::this_thread::get_id() << "::ThreadLoop() Done.");
}

void ThreadPool::AddTask(const TaskT &task){
    std::unique_lock<std::mutex> lock(this->mMutex);
    this->mTasks.push_back(task);
    this->mCond.notify_one();
}

ThreadPool::TaskT ThreadPool::Take(){
    std::unique_lock<std::mutex> lock(this->mMutex);
    while(this->mTasks.empty() && this->mIsStarted){
        DEBUG_MSG(std::this_thread::get_id() << "::WaitTask");
        this->mCond.wait(lock);
    }
    TaskT task = nullptr;
    if(!this->mTasks.empty()){
        task = this->mTasks.front();
        this->mTasks.pop_front();
        
    }
    DEBUG_MSG(std::this_thread::get_id() << "::Wakeup");
    return task;
}