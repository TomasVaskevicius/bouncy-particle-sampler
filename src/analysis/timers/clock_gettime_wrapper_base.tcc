#pragma once

#include <stdexcept>

namespace {

timespec operator-(const timespec& end, const timespec& start) {

	timespec temp;
	if (end.tv_nsec - start.tv_nsec < 0) {
		temp.tv_sec = end.tv_sec - start.tv_sec-1;
		temp.tv_nsec = 1e9 + end.tv_nsec - start.tv_nsec;
	} else {
		temp.tv_sec = end.tv_sec - start.tv_sec;
		temp.tv_nsec = end.tv_nsec - start.tv_nsec;
	}
	return temp;
}

timespec operator+(const timespec& time1, const timespec& time2) {
  timespec temp;
  if (time1.tv_nsec + time2.tv_nsec  >= 1e9) {
    temp.tv_sec = time1.tv_sec + time2.tv_sec + 1;
    temp.tv_nsec = time1.tv_nsec + time2.tv_nsec - 1e9;
  } else {
    temp.tv_sec = time1.tv_sec + time2.tv_sec;
    temp.tv_nsec = time1.tv_nsec + time2.tv_nsec;
  }
  return temp;
}

}

namespace pdmp {
namespace analysis {

void ClockGettimeWrapperBase::start() {
  if (this->isStarted_) {
    throw std::runtime_error("Called start() on an already running timer.");
  }

  this->lastStart_ = this->getTime();
  this->isStarted_ = true;
}

void ClockGettimeWrapperBase::stop() {
  if (!this->isStarted_) {
    throw std::runtime_error("Called stop() an a timer which is not running.");
  }

  this->currentTime_ = this->currentTime_ + (getTime() - this->lastStart_);
  this->isStarted_ = false;
}

void ClockGettimeWrapperBase::reset() {
  this->isStarted_ = false;
  this->currentTime_ = timespec{0, 0};
}

long ClockGettimeWrapperBase::getTimeInMs() {
  return this->currentTime_.tv_sec * 1000 + this->currentTime_.tv_nsec / 1e6;
}

}
}
