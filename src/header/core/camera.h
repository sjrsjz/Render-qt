#pragma once
#include "../Common.h"
class Camera
{

	public:
		double x, y, z;
		double x_2D, y_2D;
		double pitch/*Z*/, yaw/*Y*/, roll/*X*/;
		Camera() {

			x = y = z = 0;
			x_2D = y_2D = 0;
			pitch = yaw = roll = 0;
		}
		void setPos(double x, double y, double z) {
			this->x = x;
			this->y = y;
			this->z = z;
			this->x_2D = x;
			this->y_2D = y;
		}
		void setRot(double pitch, double yaw, double roll) {
			this->pitch = pitch;
			this->yaw = yaw;
			this->roll = roll;
		}
		void rotateX(double amt) {
			pitch += amt;
		}
		void rotateY(double amt) {
			yaw += amt;
		}
		void rotateZ(double amt) {
			roll += amt;
		}

		void moveX(double amt) { x += amt; }
		void moveY(double amt) { y += amt; }
		void moveZ(double amt) { z += amt; }
		void movePitch(double amt) { pitch += amt; }
		void moveYaw(double amt) { yaw += amt; }
		void moveRoll(double amt) { roll += amt; }
		void move(double dx, double dy, double dz) {
			//以视线方向为x轴，左手坐标系，旋转顺序为yaw,pitch,roll
			//获得旋转后的dx,dy,dz
			double dx1 = dx * cos(yaw) + dz * sin(yaw);
			double dz1 = -dx * sin(yaw) + dz * cos(yaw);
			double dx2 = dx1 * cos(pitch) - dy * sin(pitch);
			double dy2 = dx1 * sin(pitch) + dy * cos(pitch);
			double dz2 = dz1;
			x += dx2;
			y += dy2;
			z += dz2;
		}
		void moveForward(double amt) {
			double dx = amt * cos(yaw) * cos(pitch);
			double dy = amt * sin(pitch);
			double dz = amt * sin(yaw) * cos(pitch);
			move(dx, dy, dz);
		}
		void getRotationMatrix(double* m) {
			//以视线方向为x轴，左手坐标系，旋转顺序为yaw,pitch,roll
			//m[0] m[1] m[2]
			//m[3] m[4] m[5]
			//m[6] m[7] m[8]
			m[0] = cos(yaw) * cos(pitch);
			m[1] = sin(yaw) * cos(pitch);
			m[2] = -sin(pitch);
			m[3] = cos(yaw) * sin(pitch) * sin(roll) - sin(yaw) * cos(roll);
			m[4] = sin(yaw) * sin(pitch) * sin(roll) + cos(yaw) * cos(roll);
			m[5] = cos(pitch) * sin(roll);
			m[6] = cos(yaw) * sin(pitch) * cos(roll) + sin(yaw) * sin(roll);
			m[7] = sin(yaw) * sin(pitch) * cos(roll) - cos(yaw) * sin(roll);
			m[8] = cos(pitch) * cos(roll);
		}
		void getMatrix(double* m) {
			//以视线方向为x轴，左手坐标系，旋转顺序为yaw,pitch,roll
			//m[0] m[1] m[2] m[3]
			//m[4] m[5] m[6] m[7]
			//m[8] m[9] m[10] m[11]
			//m[12] m[13] m[14] m[15]
			double m1[9];
			getRotationMatrix(m1);
			m[0] = m1[0]; m[1] = m1[1]; m[2] = m1[2]; m[3] = 0;
			m[4] = m1[3]; m[5] = m1[4]; m[6] = m1[5]; m[7] = 0;
			m[8] = m1[6]; m[9] = m1[7]; m[10] = m1[8]; m[11] = 0;
			m[12] = x; m[13] = y; m[14] = z; m[15] = 1;
		}
};