#pragma once

#include "GameObject.h"
#include "Clock.h"

class Boid : public Component
{
public:
	Boid()
	{
	}

	~Boid()
	{
	}

	void Start() 
	{
		transform = owner->GetTransform();
	}

	void Update()
	{
		//Goal 1 - Avoid obstacles
		glm::vec3 accumulatedObstacleVector = { 0, 0, 0 };
		ObstacleCorrection(accumulatedObstacleVector);

		//Goal 2 - Avoid other flock
		glm::vec3 avoidOtherTribeVelocity = { 0, 0, 0 };
		AvoidOtherTribe(avoidOtherTribeVelocity);

		//Goal 3 - Avoid fellow flock
		glm::vec3 avoidFellowTribeVelocity = { 0, 0, 0 };
		AvoidFellowTribe(avoidFellowTribeVelocity);		

		//Goal 4 - Goal point accel
		goalAcceleration = glm::normalize(goalPosition - transform->GetPosition()) * goalHeuristic;

		//Goal 5 - Match motion of fellow flock
		glm::vec3 followFellowTribeVelocity = { 0, 0, 0 };
		FollowFellowTribe(followFellowTribeVelocity);

		//Final accumulation
		velocity = goalAcceleration + accumulatedObstacleVector + avoidOtherTribeVelocity + avoidFellowTribeVelocity + followFellowTribeVelocity;
		
		transform->Translate(velocity * Clock::fixedDeltaTime);
	}

	void SetGoal(glm::vec3 newGoal) { goalPosition = newGoal; }

	glm::vec3 GetVelocity() { return velocity; }

	void AddFellowTribeMember(Boid* tribesman) { fellowTribe.push_back(tribesman); }
	void AddOtherTribeMember(Boid* tribesman) { otherTribe.push_back(tribesman); }
	void AddObstacle(Transform* obstacle) { obstacles.push_back(obstacle); }

private:
	Transform* transform;

	glm::vec3 goalPosition;
	glm::vec3 velocity;

	glm::vec3 goalAcceleration; 
	float goalHeuristic = 0.03f;


	float obstacleDistance = 3.0f; float obstacleHeuristic = 1.0f;
	inline void ObstacleCorrection(glm::vec3 &accumulatedVector)
	{
		glm::vec3 distanceVector;
		for (Transform* obstacle : obstacles)
		{
			distanceVector = transform->GetPosition() - obstacle->GetPosition();
			float distanceLength = glm::length(distanceVector);
			if (distanceLength < obstacleDistance)
			{
				accumulatedVector += glm::normalize(distanceVector) * obstacleHeuristic;
			}
		}
	}
	

	float otherTribeDistance = 3.0f,  OtherTribeHeuristic = 0.035f;
	inline void AvoidOtherTribe(glm::vec3 &accumulatedVector)
	{
		glm::vec3 distanceVector;
		for (Boid* otherTribeMember : otherTribe)
		{
			distanceVector = transform->GetPosition() - otherTribeMember->owner->GetTransform()->GetPosition();
			float distanceLength = glm::length(distanceVector);
			if (distanceLength < otherTribeDistance)
			{
				accumulatedVector += glm::normalize(distanceVector) * OtherTribeHeuristic;
			}
		}
	}


	float fellowTribeDistance = 3.0f, AvoidTribesmanHeuristic = 0.5f;
	inline void AvoidFellowTribe(glm::vec3 &accumulatedVector)
	{
		glm::vec3 distanceVector;
		for (Boid* fellowTribeMember : fellowTribe)
		{
			distanceVector = transform->GetPosition() - fellowTribeMember->owner->GetTransform()->GetPosition();
			float distanceLength = glm::length(distanceVector);
			if (distanceLength < fellowTribeDistance)
			{
				accumulatedVector += glm::normalize(distanceVector) * OtherTribeHeuristic;
			}
		}
	}


	float followTribesmanHeuristic = 0.4f;
	inline void FollowFellowTribe(glm::vec3 &accumulatedVector)
	{
		glm::vec3 distanceVector;
		for (Boid* fellowTribeMember : fellowTribe)
		{
			distanceVector = transform->GetPosition() - fellowTribeMember->owner->GetTransform()->GetPosition();
			float distance = glm::length(distanceVector);
			accumulatedVector += fellowTribeMember->GetVelocity() * followTribesmanHeuristic;
		}
		accumulatedVector /= fellowTribe.size();
	}
	
	std::vector<Boid*> fellowTribe; 
	std::vector<Boid*> otherTribe;
	std::vector<Transform*> obstacles;

};