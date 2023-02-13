#include <iostream> /* for cout */
#include <glut.h> /* declare always last for exit(0) error */
#include <array>
#include <vector>

using namespace std;

// Represents a particle of the system.
struct particle {
	int id; // Unique id of the particle
	float mass;
	float expire_time; // The time when the particle should be destroyed
	array<float, 3> position;
	array<float, 3> speed;
};

// Set constants
const float BACKGROUND_COLOR[4] = { 0.78, 0.94, 1.0, 1.0 };
const float PLANE_SIZE = 600; // Plane side width
const float PART_LIFE_SPAN = 10; // The total life of every particle in seconds
const float PART_START_POS[3] = { 0, 100, 0 }; // The starting position of every particle
const float MIN_SPEED = 10.0; // The minimum initial speed (in a given dimension) of a particle
const float MAX_SPEED = 100.0; // The maximum initial speed (at a dimension) of a particle
const float GEN_REFRESH = 0.05; // The refresh rate of particles generator
const float PART_SYST_REFRESH = 0.01; // The refresh rate of the particle system
const float FPS = 60; // Frames per second (how often to display changes on screen)
const float GRAVITY[3] = { 0, -500, 0 }; // The gravity vector
const float WIND[3] = { -400, 0, 0 }; // The wind vector
const float MIN_WIND_Y = 30; // The minimum level (height) the wind is applied
const float MAX_WIND_Y = 60; // The maximum level (height) the wind is applied
const float PARTICLE_RADIUS = 4; // The radius of the particles' sphere

float timepart; // The current time of the particle system
int particles_count; // The total amount of particles that have been generated
vector<particle> particles;

bool hasWind; // If wind affects particles
bool isGenerating; // If generator is generating particles

// Declare functions
void generatorTrigger(int);
void partSystemTrigger(int);
void redisplayTrigger(int);

void initEnvironment()
{
	// Attributes
	glEnable(GL_BLEND);
	glClearColor(BACKGROUND_COLOR[0], BACKGROUND_COLOR[1], BACKGROUND_COLOR[2], BACKGROUND_COLOR[2]); // Set background color
	glLoadIdentity();

	// Set camera view
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60, 1.0, 20, 600);
	gluLookAt(0, 140, -400, 0, 80, 0, 0.0, 1.0, 0.0);
	glMatrixMode(GL_MODELVIEW);

	// Initialize and set triggers
	timepart = 0;
	particles_count = 0;
	hasWind = false;
	isGenerating = true;
	generatorTrigger(0);
	partSystemTrigger(0);
	redisplayTrigger(0);
}

// Generates a random speed value inside the limits (with positive or negative values)
float generateSpeed()
{
	// Calculate random number between min and max
	float speed = MIN_SPEED + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (MAX_SPEED - MIN_SPEED)));
	// Randomly (no way to know if it will be even or odd) set if speed will be negative or positive
	if ((int(speed) % 2 == 0))
	{
		return speed;
	}
	else
	{
		return -speed;
	}
}

// Generates a particle with random speed.
void generateParticles()
{
	particles_count++;
	particle p = particle{ particles_count, 1.0, timepart + PART_LIFE_SPAN, {PART_START_POS[0], PART_START_POS[1] , PART_START_POS[2] }, { generateSpeed(), generateSpeed(), generateSpeed() } };
	particles.push_back(p);
	cout << "P: " << particles.size() << endl;
}

// Updates the position and speed of the particle.
void updateParticle(particle& particle)
{
	// Set the force to be applied on the particle (gravity and wind)
	float force[3] = { GRAVITY[0], GRAVITY[1] , GRAVITY[2] };
	if (hasWind && particle.position[1] >= MIN_WIND_Y && particle.position[1] <= MAX_WIND_Y)
	{
		force[0] += WIND[0];
		force[1] += WIND[1];
		force[2] += WIND[2];
	}
	// Set time interval (step)
	float step = PART_SYST_REFRESH;
	bool collision = false;
	// If particle is going to collide with the terrain, find the time interval of collision
	if (particle.position[1] + step * particle.speed[1] < 0)
	{
		step = -particle.position[1] / particle.speed[1];
		collision = true;
	}
	// Calculate new positions and speeds
	for (int i = 0; i < 3; i++)
	{
		particle.position[i] += step * particle.speed[i];
		particle.speed[i] += step * force[i] / particle.mass;
	}
	// If a collision happened, reverse the y coordinate of speed
	if (collision)
	{
		particle.speed[1] = -particle.speed[1];
	}
}

// Responsible for particles. Removes the expired ones and updates the position of the others.
void particleSystemUpdate()
{
	if (particles.size() == 0)
	{
		return;
	}
	// Update every particle
	for (auto it = particles.begin(); it < particles.end(); ++it)
	{
		// If particle expired, remove it
		if (timepart >= (*it).expire_time)
		{
			it = particles.erase(it);
			if (particles.size() == 0)
			{
				return;
			}
		}
		else
		{
			updateParticle((*it));
		}
	}
}

// Triggers the particles generator in time intervals.
void generatorTrigger(int)
{
	if (isGenerating)
	{
		generateParticles();
	}
	glutTimerFunc(GEN_REFRESH * 1000, generatorTrigger, 0);
}

// Triggers the particle system in time intervals.
void partSystemTrigger(int)
{
	particleSystemUpdate();
	glutTimerFunc(PART_SYST_REFRESH * 1000, partSystemTrigger, 0);
	timepart += PART_SYST_REFRESH;
}

// Displays changes to the screen in time intervals.
void redisplayTrigger(int)
{
	glutPostRedisplay();
	glutTimerFunc(1000.0 / FPS, redisplayTrigger, 0);
}

// Displays particles and a plane at zero height.
void displayParticles(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  // Clear the window
	glLoadIdentity();

	// Draw plane
	glColor3ub(0, 170, 51);
	glBegin(GL_QUADS);
	glVertex3f(-PLANE_SIZE / 2, -PARTICLE_RADIUS, -PLANE_SIZE / 2);
	glVertex3f(-PLANE_SIZE / 2, -PARTICLE_RADIUS, PLANE_SIZE / 2);
	glVertex3f(PLANE_SIZE / 2, -PARTICLE_RADIUS, PLANE_SIZE / 2);
	glVertex3f(PLANE_SIZE / 2, -PARTICLE_RADIUS, -PLANE_SIZE / 2);
	glEnd();

	// Draw generator point
	glColor3ub(105, 0, 119);
	glEnable(GL_POINT_SMOOTH);
	glEnableClientState(GL_VERTEX_ARRAY);
	glPointSize(5); // Set size
	glVertexPointer(3, GL_FLOAT, 0, PART_START_POS); // Set point dimensions
	glDrawArrays(GL_POINTS, 0, 1); // Draw the proper number of points
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisable(GL_POINT_SMOOTH);

	// Draw particles
	for (auto it = particles.begin(); it < particles.end(); ++it)
	{
		particle p = *it;
		// Set color of particle based on expiration time
		float red = 255 * (p.expire_time - timepart) / PART_LIFE_SPAN;
		glColor3ub(red, 31, 31);
		glLoadIdentity();
		// Set position of particle
		glTranslatef(p.position[0], p.position[1], p.position[2]);
		glutSolidSphere(PARTICLE_RADIUS, 16, 16);
	}

	glutSwapBuffers(); // Swap buffers
	return;
}