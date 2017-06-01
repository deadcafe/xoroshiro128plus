/*
 * Copyright (c) 2017, deadcafe.beef@gmail.com
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2.Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the distribution.
 * 3.Neither the name of the project nor the names of its contributors
 *   may be used to endorse or promote products derived from this software
 *   without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * see also http://xoroshiro.di.unimi.it/
 */

#include "xoroshiro128plus.h"

static inline uint64_t
rotl(const uint64_t x,
     unsigned k)
{
        return (x << k) | (x >> (64 - k));
}

static inline uint64_t
next(struct random_state_s *state)
{
        const uint64_t s0 = state->s[0];
        uint64_t       s1 = state->s[1];
        const uint64_t result = s0 + s1;

        s1 ^= s0;
        state->s[0] = rotl(s0, 55) ^ s1 ^ (s1 << 14);
        state->s[1] = rotl(s1, 36);

        return result;
}

static inline void
jump(struct random_state_s *state)
{
        static const uint64_t JUMP[] = {
                0xbeac0467eba5facb,
                0xd86b048b86aa9922,
        };

	uint64_t s0 = 0;
	uint64_t s1 = 0;

	for (unsigned i = 0; i < sizeof JUMP / sizeof *JUMP; i++) {
		for (int b = 0; b < 64; b++) {
			if (JUMP[i] & UINT64_C(1) << b) {
				s0 ^= state->s[0];
				s1 ^= state->s[1];
			}
			next(state);
		}
        }

	state->s[0] = s0;
	state->s[1] = s1;
}

uint64_t
random_get(struct random_state_s *state)
{
        return next(state);
}

void
random_seed(struct random_state_s *state,
            uint64_t seed)
{
        state->s[0] = seed;
        state->s[1] = 0;
        jump(state);
}

