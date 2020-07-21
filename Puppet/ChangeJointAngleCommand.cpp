//
//  ChangeJointAngleCommand.cpp
//  A3
//
//  Created by Jessica's Mac on 2017-10-21.
//  Copyright © 2017 none. All rights reserved.
//

#include "ChangeJointAngleCommand.hpp"
using namespace std;

ChangeJointAngleCommand::ChangeJointAngleCommand(JointNode* node):m_node(node)
{
    reset();
}

void ChangeJointAngleCommand::reset()
{
    m_angleXs.clear();
    m_angleYs.clear();
    // Put the init angles onto the stack
    m_angleXs.push_back(m_node->m_joint_x.init);
    m_angleYs.push_back(m_node->m_joint_y.init);
    m_currentXIdx = 0;
    m_currentYIdx = 0;
}

void ChangeJointAngleCommand::updateStack()
{
    // update stack
    m_angleXs.push_back(m_node->m_joint_x.current);
    m_currentXIdx = m_angleXs.size() - 1;
    
    m_angleYs.push_back(m_node->m_joint_y.current);
    m_currentYIdx = m_angleYs.size() - 1;
}

void ChangeJointAngleCommand::undo()
{
    double angleX, angleY = 0.0;
    if (m_currentXIdx > 0) {
        double prevAngleX = m_angleXs[m_currentXIdx - 1];
        double currentAngleX = m_angleXs[m_currentXIdx];
        angleX = prevAngleX - currentAngleX;
        // update current stack
        m_currentXIdx = m_currentXIdx - 1;
    } else
	angleX = 0.0;

    if (m_currentYIdx > 0) {
        double prevAngleY = m_angleYs[m_currentYIdx - 1];
        double currentAngleY = m_angleYs[m_currentYIdx];
        angleY = prevAngleY - currentAngleY;
        // update current stack
        m_currentYIdx = m_currentYIdx - 1;
    } else
	angleY = 0.0;
    cout << "undo " << m_node->m_name << endl;
    cout << "m_currentXIdx " << m_currentXIdx << endl;
    cout << "m_currentYIdx " << m_currentYIdx << endl;
    rotateJoint(angleX, angleY);
}

void ChangeJointAngleCommand::redo()
{
    double angleX, angleY = 0.0;
    if (m_currentXIdx + 1 < m_angleXs.size()) {
        double laterAngleX = m_angleXs[m_currentXIdx + 1];
        double currentAngleX = m_angleXs[m_currentXIdx];
        angleX = laterAngleX - currentAngleX;
        // update current stack
        m_currentXIdx = m_currentXIdx + 1;
    } else
	angleX = 0.0;
    
    if (m_currentYIdx + 1 < m_angleYs.size()) {
        double laterAngleY = m_angleYs[m_currentYIdx + 1];
        double currentAngleY = m_angleYs[m_currentYIdx];
        angleY = laterAngleY - currentAngleY;
        // update current stack
        m_currentYIdx = m_currentYIdx + 1;
    } else
	angleY = 0.0;
cout << "redo " << m_node->m_name << endl;
    cout << "m_currentXIdx " << m_currentXIdx << endl;
    cout << "m_currentYIdx " << m_currentYIdx << endl;
    rotateJoint(angleX, angleY);
}

void ChangeJointAngleCommand::rotateJoint (double angleX, double angleY) {
    
    // check joint_x limit
    double currentJointX = m_node->m_joint_x.current;
    if (currentJointX + angleX < m_node->m_joint_x.min) {
        angleX = m_node->m_joint_x.min - currentJointX;
    } else if (currentJointX + angleX > m_node->m_joint_x.max) {
        angleX = m_node->m_joint_x.max - currentJointX;
    }
    m_node->rotate('x', angleX);
    m_node->m_joint_x.current += angleX;
    
    // check joint_y limit
    double currentJointY = m_node->m_joint_y.current;
    if (currentJointY + angleY < m_node->m_joint_y.min) {
        angleY = m_node->m_joint_y.min - currentJointY;
    } else if (currentJointY + angleY > m_node->m_joint_y.max) {
        angleY = m_node->m_joint_y.max - currentJointY;
    }
    m_node->rotate('y', angleY);
    m_node->m_joint_y.current += angleY;
}
