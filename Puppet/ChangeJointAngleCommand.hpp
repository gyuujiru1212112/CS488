//
//  ChangeJointAngleCommand.hpp
//  A3
//
//  Created by Jessica's Mac on 2017-10-21.
//  Copyright © 2017 none. All rights reserved.
//

#ifndef ChangeJointAngleCommand_hpp
#define ChangeJointAngleCommand_hpp

#include <stdio.h>
#include <vector>

#include "JointNode.hpp"


class ChangeJointAngleCommand
{
    public:
    
    ChangeJointAngleCommand(JointNode* node);
    ~ChangeJointAngleCommand() {}
     void rotateJoint(double angleX, double angleY);
    void updateStack();
    void undo();
    void redo();
    void reset();
    
    JointNode* m_node;
    
    size_t m_currentXIdx;
    size_t m_currentYIdx;
    std::vector<double> m_angleXs;
    std::vector<double> m_angleYs;
};

#endif /* ChangeJointAngleCommand_hpp */
