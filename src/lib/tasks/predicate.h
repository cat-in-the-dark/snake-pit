#ifndef PREDICATE_H
#define PREDICATE_H

#include <functional>

class PredicateTask {
public:
  PredicateTask(std::function<bool()> &&predicate,
                std::function<void()> &&callback);

  bool Update();

  void Reset();

private:
  std::function<bool()> predicate_;

  std::function<void()> callback_;

  bool wasFired_;
};

#endif /* PREDICATE_H */
