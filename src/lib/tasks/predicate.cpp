#include "predicate.h"

PredicateTask::PredicateTask(std::function<bool()> &&predicate,
                             std::function<void()> &&callback)
    : predicate_(std::move(predicate)), callback_(std::move(callback)),
      wasFired_(false) {}

bool PredicateTask::Update() {
  if (wasFired_) {
    return false;
  }

  if (predicate_()) {
    callback_();
    wasFired_ = true;
    return true;
  }

  return false;
}

void PredicateTask::Reset() { wasFired_ = false; }
