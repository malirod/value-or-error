// Copyright [2019] <Malinovsky Rodion> (rodionmalino@gmail.com)
#pragma once

#include <string>
#include <vector>

#include "value_or_error.h"

namespace rms {

class DBManager {
 public:
  explicit DBManager(bool do_interrupt);

  rms::ValueOrError<std::vector<std::string>> get_customers() const;

 private:
  const bool m_do_interrupt;
};

}  // namespace rms
