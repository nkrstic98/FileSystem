#pragma once

#include <vector>
#include <stdint.h>
#include "part.h"
#include "file.h"

using namespace std;

const unsigned int FNAMELEN = 8;
const unsigned int FEXTLEN = 3;
const unsigned int DIRNUM = ClusterSize / 32;
const unsigned int BITNUM = ClusterSize * 8;
const unsigned int INDEXSIZE = ClusterSize / 4;
const unsigned long long int MAXSIZE = 2048 * 512 * 512;