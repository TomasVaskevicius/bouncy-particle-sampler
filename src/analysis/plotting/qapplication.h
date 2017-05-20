#pragma once

#include <QApplication>

int mockArgc = 1;
const char* mockArgv[] = {"pdmp-monte-carlo"};
QApplication qApplication(mockArgc, const_cast<char**>(mockArgv));


