#include <stdio.h>
#include <stdint.h>
#include "transfer.h"

void main() {
	int x, y;
	for (x=0; x<16; x++) {
		for (y=0; y<24; y++) {
			int B = (y / 8) + 3 * x;
			int b = 7 - (y % 8);
			if (B >= 24) {
				B = 47 - B;
				b = 7 - b;
			}
			printf(((transf_icon[B] & (1<<b)) != 0) ? "XX" : "  ");
		}
		printf("\n");
	}
}
