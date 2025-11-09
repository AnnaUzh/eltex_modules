#pragma once

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

char check_ip_mask(char ipv[]);
void generate_ip_and_check(char ipv[], char subnet_mask[], int n);
