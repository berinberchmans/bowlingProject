/*-----------------------------------------------------------
  Simulation Header File
  -----------------------------------------------------------*/
#include"vecmath.h"

/*-----------------------------------------------------------
  Macros
  -----------------------------------------------------------*/
#define TABLE_X			(0.6f) 
#define TABLE_Z			(4.0f)
#define TABLE_Y			(0.01f)
#define BALL_RADIUS		(0.05f)
#define BALL_MASS		(0.15f)
#define PIN_MASS		(0.1f)
#define TWO_PI			(6.2832f)
#define	SIM_UPDATE_MS	(10)
#define NUM_BALLS		(11)		
#define NUM_CUSHIONS	(5)
#define NUM_CUSHIONS_lane (10)		
#define MAX_PARTICLES	(200)
#define NUM_PLAYERS	(2)
/*-----------------------------------------------------------
  plane normals
  -----------------------------------------------------------*/
/*
extern vec2	gPlaneNormal_Left;
extern vec2	gPlaneNormal_Top;
extern vec2	gPlaneNormal_Right;
extern vec2	gPlaneNormal_Bottom;
*/


/*-----------------------------------------------------------
  cushion class
  -----------------------------------------------------------*/
class cushion
{
public:
	vec2	vertices[2]; //2d
	vec2	centre;
	vec2	normal;

	void MakeNormal(void);
	void MakeCentre(void);
};


/*-----------------------------------------------------------
  player class
  -----------------------------------------------------------*/

class player {
	static int playerIndex;
public:
	int score;
	int index;
	player() : score(0) {
		index = playerIndex++;
	}
	//void getPlayer(int playerI);
};

/*-----------------------------------------------------------
  ball class
  -----------------------------------------------------------*/

class ball
{
	static int ballIndexCnt;
public:
	vec2	position;
	vec2	velocity;
	float	radius;
	float	mass;
	int		index;
	int	touched;

	ball(): position(0.0), velocity(0.0), radius(BALL_RADIUS), 
		mass(BALL_MASS),touched(0) {index = ballIndexCnt++;
	if (index != 0) {
		mass = PIN_MASS;
	}
	Reset();}
	
	void Reset(void);
	void ApplyImpulse(vec2 imp);
	void ApplyFrictionForce(int ms);
	void DoPlaneCollision(const cushion &c);
	void DoBallCollision(ball &b);
	void Update(int ms);
	
	bool HasHitPlane(const cushion &c) const;
	bool HasHitBall(const ball &b) const;

	void HitPlane(const cushion &c);
	void HitBall(ball &b);
};



class particle 
{
public:
	vec3 position;
	vec3 velocity;

	particle() {position=0;velocity=0;}
	void update(int ms);
};



class particleSet 
{
public:
	particle *particles[MAX_PARTICLES];
	int num;

	particleSet()
	{
		for(int i=0;i<MAX_PARTICLES;i++) particles[i] = 0;
		num=0;
	}

	~particleSet()
	{
		for(int i=0;i<MAX_PARTICLES;i++)
		{
			if(particles[i]) delete particles[i];
		}
	}

	void AddParticle(const vec3 &pos);
	void update(int ms);
};


/*-----------------------------------------------------------
  table class
  -----------------------------------------------------------*/
class table
{
public:
	ball balls[NUM_BALLS];	
	cushion cushions[NUM_CUSHIONS];
	particleSet parts;
	player players[NUM_PLAYERS];
	void SetupCushions(void);
	void Update(int ms);	
	bool AnyBallsMoving(void) const;
};

/*-----------------------------------------------------------
  lane class
  -----------------------------------------------------------*/
class lane
{
public:
	ball balls[NUM_BALLS];
	cushion cushions[NUM_CUSHIONS];
	particleSet parts;

	void SetupCushions(void);
	void Update(int ms);
	bool AnyBallsMoving(void) const;
};

/*-----------------------------------------------------------
  global table
  -----------------------------------------------------------*/
extern lane gLane;
extern table gTable;


