#ifndef COLLISION_HPP
#define COLLISION_HPP

#include "vectors.hpp"

bool test_collision(Fvec lhs_pos, Fvec lhs_size, Fvec rhs_pos, Fvec rhs_size);
void handle_collision_movingAA(Fvec& lhs_pos, Fvec lhs_size, Fvec& lhs_vel,
							   Fvec rhs_pos, Fvec rhs_size);
void handle_collision_movingAABB(Fvec& lhs_pos, Fvec lhs_size, Fvec& lhs_vel,
								 Fvec& rhs_pos, Fvec rhs_size, Fvec& rhs_vel);
#endif
