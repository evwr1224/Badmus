void reset();
int drivePos();
bool isDriving();

int driveTask();
int turnTask();

void driveAsync(double sp);
void turnAsync(double sp);
void drive(double sp);
void turn(double sp);
void slowDrive(double sp, double dp = .1);

void setSpeed(int speed);
void setBrakeMode(int mode);

void tankOp();
void arcadeOp();

void initDrive();

void delay(int sleepTime);
