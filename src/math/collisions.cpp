#include "collisions.hpp"

bool test_collision(Fvec lhs_pos, Fvec lhs_size, Fvec rhs_pos, Fvec rhs_size)
{
	Fvec lhs_bottom_right = lhs_pos + lhs_size;
	return ((lhs_pos.x <= rhs_pos.x && rhs_pos.x <= lhs_bottom_right.x) ||
			(rhs_pos.x <= lhs_pos.x && lhs_pos.x <= rhs_pos.x + rhs_size.x)) &&
		   ((lhs_pos.y <= rhs_pos.y && rhs_pos.y <= lhs_bottom_right.y) ||
			(rhs_pos.y <= lhs_pos.y && lhs_pos.y <= rhs_pos.y + rhs_size.y));
}

void handle_collision_movingAA(Fvec& lhs_pos, Fvec lhs_size, Fvec& lhs_vel,
							   Fvec rhs_pos, Fvec rhs_size)
{
	float lhs_bottom = lhs_pos.y + lhs_size.y;
	float rhs_bottom = rhs_pos.y + rhs_size.y;
	float lhs_right = lhs_pos.x + lhs_size.x;
	float rhs_right = rhs_pos.x + rhs_size.x;

	float b_collision = rhs_bottom - lhs_pos.y;
	float t_collision = lhs_bottom - rhs_pos.y;
	float l_collision = lhs_right - rhs_pos.x;
	float r_collision = rhs_right - lhs_pos.x;

	if(t_collision < b_collision && t_collision < l_collision &&
	   t_collision < r_collision)
	{
		// Top collision
		if(lhs_vel.y > 0.0f)
		{
			lhs_vel.y = 0.0f;
		}
		while(lhs_pos.y + lhs_size.y > rhs_pos.y)
		{
			lhs_pos.y -= 1;
		}
	}
	else if(b_collision < t_collision && b_collision < l_collision &&
			b_collision < r_collision)
	{
		// bottom collision
		if(lhs_vel.y < 0.0f)
		{
			lhs_vel.y = 0.0f;
		}
		while(lhs_pos.y < rhs_pos.y + rhs_size.y)
		{
			lhs_pos.y += 1;
		}
	}
	else if(l_collision < r_collision && l_collision < t_collision &&
			l_collision < b_collision)
	{
		// Left collision
		if(lhs_vel.x > 0.0f)
		{
			lhs_vel.x = 0.0f;
		}
		while(lhs_pos.x + lhs_size.x > rhs_pos.x)
		{
			lhs_pos.x -= 1;
		}
	}
	else if(r_collision < l_collision && r_collision < t_collision &&
			r_collision < b_collision)
	{
		// Right collision
		if(lhs_vel.x < 0.0f)
		{
			lhs_vel.x = 0.0f;
		}
		while(lhs_pos.x < rhs_pos.x + rhs_size.x)
		{
			lhs_pos.x += 1;
		}
	}
}

void handle_collision_movingAABB(Fvec& lhs_pos, Fvec lhs_size, Fvec& lhs_vel,
								 Fvec& rhs_pos, Fvec rhs_size, Fvec& rhs_vel)
{
}
