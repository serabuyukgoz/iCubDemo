#include <stdio.h>
#include <yarp/os/all.h>
#include <yarp/sig/all.h>
#include <yarp/dev/all.h>
#include <string>

using namespace yarp::os;
using namespace yarp::sig;
using namespace yarp::dev;
using namespace std;

int main() {
	bool ReadDone = 0;
	bool Fullwave = 0;
	bool WriteDone = 0;
	char WaveCount = 5;

	Network yarp; // set up yarp

	Property options; //create device options
	options.put("device", "remote_controlboard");
	options.put("local", "/tutorial/motor/client");
	options.put("remote", "/icubSim/right_arm");

	PolyDriver robotDevice(options); //create driver
	if (!robotDevice.isValid())
	{
			printf("Device not available. Here are the known devices:\n");
			printf("%s", Drivers::factory().toString().c_str());
			return 1;
	}

	IPositionControl *pos;
	IVelocityControl *vel;
	IEncoders *enc;

	IControlMode2 *ictrl;

	robotDevice.view(pos);
	robotDevice.view(vel);
	robotDevice.view(enc);
	robotDevice.view(ictrl);

	if(pos == NULL || vel == NULL || enc == NULL || ictrl == NULL)
	{
			printf("Error getting IPOsitionContorl interface.\n");
			robotDevice.close();
			return 1;
	}

	//get amount of joints, in this case the arm
	int jnts = 0;
	pos->getAxes(&jnts);
	printf("jnts = %d \n", jnts);

	Vector setpoints;
	Vector RefSpeed;
	Vector Encoders;
	Vector RefAcceleration;
	Vector CurrentPos;

	setpoints.resize(jnts);
	RefSpeed.resize(jnts);
	RefAcceleration.resize(jnts);
	Encoders.resize(jnts);
	CurrentPos.resize(1);

	for (int i = 0; i < jnts; i++)
	{
			ictrl->setControlMode(i,VOCAB_CM_POSITION);
			RefSpeed[i] = 25;
			RefAcceleration[i] = 30;
			setpoints[i] = 0;
	}

	pos->setRefSpeeds(RefSpeed.data());
	pos->setRefAccelerations(RefAcceleration.data());

	printf("Waiting for encoders to become available...");
	while(!(enc->getEncoders(Encoders.data())))
	{
			Time::delay(0.1);
			printf(" . ");
	}

	printf("\n Encoders Found \n");

	for (int i = 0; i < jnts; i++)
	{
			printf("Encoder value %d = %g \n",i,Encoders[i]);
	}

	int opt;

	cout<<"Chose Option: ";
	cin>>opt;
	while(opt != 0)
	{

		if (opt == 1){
			setpoints[0] = -90;
			setpoints[1] = 90;
			setpoints[2] = 0;
			setpoints[3] = 90;

			WriteDone = pos->positionMove(setpoints.data());

			while (WaveCount > 0)
			{
					if(ReadDone)
					{
							if(Encoders[3]>= 85)
							{
									WriteDone = pos->positionMove(3,25);
									if(Fullwave)
									{
											WaveCount--;
											Fullwave = 0;
									}
							}

							if(Encoders[3]<=30)
							{
									WriteDone = pos->positionMove(3,90);
									Fullwave = 1;
							}
					}

					if (WriteDone)
					{
							ReadDone=enc->getEncoders(Encoders.data());
					}
			}
			printf("i wove %d times, i am done for today goodby.... \n", WaveCount);
			WaveCount = 5;
		}
		cout<<"Chose Option: ";
		cin>>opt;
	}

	cout<<"I'm Done!";
	robotDevice.close();

}
