#include <cmath>
#include <cstdlib>
#include "visualizer.h"
#include <iostream>
using namespace std;
using namespace Utils;

Visualizer::Visualizer(Mediator* owner,Settings* ss) {
	lts = NULL;
	mediator = owner;
	settings = ss;
	settings->subscribe(BranchRotation,this);
	settings->subscribe(BranchTilt,this);
	settings->subscribe(InterpolateColor1,this);
	settings->subscribe(InterpolateColor2,this);
	settings->subscribe(LongInterpolation,this);
	settings->subscribe(MarkedColor,this);

	visObjectFactory = new VisObjectFactory();
	primitiveFactory = new PrimitiveFactory(settings);
	sin_obt = float(sin(deg_to_rad(settings->getInt(BranchTilt))));
	cos_obt = float(cos(deg_to_rad(settings->getInt(BranchTilt))));

	visStyle = CONES;
	update_matrices = false;
	update_colors = false;
	create_objects = false;
}

Visualizer::~Visualizer() {
	delete visObjectFactory;
	delete primitiveFactory;
}

float Visualizer::getHalfStructureHeight() const {
	if (lts == NULL) {
		return 0.0f;
	}
	return settings->getFloat(ClusterHeight)*(lts->getNumRanks()-1) / 2.0f;
}

void Visualizer::setLTS(LTS* l) {
	lts = l;
	float ratio = lts->getInitialState()->getCluster()->getSize() /
		(lts->getNumRanks()-1);
	settings->setFloat(ClusterHeight,max(4,round_to_int(40.0f * ratio)) / 10.0f);
	traverseTree(true);
}

Utils::VisStyle Visualizer::getVisStyle() const {
	return visStyle;
}

void Visualizer::setMarkStyle(Utils::MarkStyle ms) {
	markStyle = ms;
	update_colors = true;
}

void Visualizer::setVisStyle(Utils::VisStyle vs) {
	if (visStyle != vs) {
		visStyle = vs;
		traverseTree(true);
	}
}

void Visualizer::notify(SettingID s) {
	switch (s) {
		case BranchTilt:
			sin_obt = float(sin(deg_to_rad(settings->getInt(BranchTilt))));
			cos_obt = float(cos(deg_to_rad(settings->getInt(BranchTilt))));
			update_matrices = true;
			break;
		case BranchRotation:
			update_matrices = true;
			break;
		case InterpolateColor1:
		case InterpolateColor2:
		case LongInterpolation:
			if (markStyle == NO_MARKS) {
				update_colors = true;
			}
			break;
		case MarkedColor:
			if (markStyle != NO_MARKS) {
				update_colors = true;
			}
		default:
			break;
	}
}

void Visualizer::computeBoundsInfo(float &bcw,float &bch) {
	bcw = 0.0f;
	bch = 0.0f;
	if (lts != NULL) {
		computeSubtreeBounds(lts->getInitialState()->getCluster(),bcw,bch);
	}
}

void Visualizer::computeSubtreeBounds(Cluster* root,float &bw,float &bh) {
	// compute the bounding cylinder of the structure.
	if (!root->hasDescendants()) {
		bw = root->getTopRadius();
		bh = 2.0f * root->getTopRadius();
	}
	else {
		Cluster *desc;
		int i;
		for (i = 0; i < root->getNumDescendants(); ++i) {
			desc = root->getDescendant(i);
			if (desc->getPosition() < -0.9f) {
				// descendant is centered
				float dw = 0.0f;
				float dh = 0.0f;
				computeSubtreeBounds(desc,dw,dh);
				bw = max(bw,dw);
				bh = max(bh,dh);
			}
			else {
				float dw = 0.0f;
				float dh = 0.0f;
				computeSubtreeBounds(desc,dw,dh);
				bw = max(bw,root->getBaseRadius() + dh*sin_obt + dw*cos_obt);
				bh = max(bh,dh*cos_obt + dw*sin_obt);
			}
		}
		bw = max(bw,root->getTopRadius());
		bh += settings->getFloat(ClusterHeight);
	}
}

// ------------- STRUCTURE -----------------------------------------------------

void Visualizer::drawStructure() {
	if (lts == NULL) {
		return;
	}
	if (update_matrices) {
		traverseTree(false);
		update_matrices = false;
	}
	if (update_colors) {
		updateColors();
		update_colors = false;
	}
	visObjectFactory->drawObjects(primitiveFactory,settings->getUByte(Alpha));
}

void Visualizer::traverseTree(bool co) {
	if (lts == NULL) {
		return;
	}
	create_objects = co;
	if (co) {
		visObjectFactory->clear();
		update_colors = true;
	}
	glPushMatrix();
	glLoadIdentity();
	switch (visStyle) {
		case CONES:
			traverseTreeC(lts->getInitialState()->getCluster(),true,0);
			break;
		case TUBES:
			traverseTreeT(lts->getInitialState()->getCluster(),0);
			break;
		default:
			break;
	}
	glPopMatrix();
}

void Visualizer::traverseTreeC(Cluster *root,bool topClosed,int rot) {
	if (!root->hasDescendants()) {
		float r = root->getTopRadius();
		glPushMatrix();
			glScalef(r,r,r);
			if (create_objects) {
				root->setVisObject(visObjectFactory->makeObject(
													 primitiveFactory->makeSphere()));
			} else {
				visObjectFactory->updateObjectMatrix(root->getVisObject());
			}
		glPopMatrix();
	}
	else {
		int drot = rot + settings->getInt(BranchRotation);
		if (drot >= 360) {
			drot -= 360;
		}
		
		glTranslatef(0.0f,0.0f,settings->getFloat(ClusterHeight));
		for (int i = 0; i < root->getNumDescendants(); ++i) {
			Cluster* desc = root->getDescendant(i);
			if (desc->getPosition() < -0.9f) {
				if (root->getNumDescendants() > 1) {
					traverseTreeC(desc,false,drot);
				} else {
					traverseTreeC(desc,false,rot);
				}
			} else {
				glRotatef(-desc->getPosition()-rot,0.0f,0.0f,1.0f);
				glTranslatef(root->getBaseRadius(),0.0f,0.0f);
				glRotatef(settings->getInt(BranchTilt),0.0f,1.0f,0.0f);
				traverseTreeC(desc,true,drot);
				glRotatef(-settings->getInt(BranchTilt),0.0f,1.0f,0.0f);
				glTranslatef(-root->getBaseRadius(),0.0f,0.0f);
				glRotatef(desc->getPosition()+rot,0.0f,0.0f,1.0f);
			}
		}
		glTranslatef(0.0f,0.0f,-settings->getFloat(ClusterHeight));
		
		float r = root->getBaseRadius() / root->getTopRadius();
		glPushMatrix();
		glTranslatef(0.0f,0.0f,0.5f*settings->getFloat(ClusterHeight));
		if (r > 1.0f) {
			r = 1.0f / r;
			glRotatef(180.0f,1.0f,0.0f,0.0f);
			glScalef(root->getBaseRadius(),root->getBaseRadius(),settings->getFloat(ClusterHeight));
			if (create_objects) {
				root->setVisObject(visObjectFactory->makeObject(
						primitiveFactory->makeTruncatedCone(r,topClosed,
						root->getNumDescendants() > 1)));
			} else {
				visObjectFactory->updateObjectMatrix(root->getVisObject());
			}
		} else {
			glScalef(root->getTopRadius(),root->getTopRadius(),settings->getFloat(ClusterHeight));
			if (create_objects) {
				root->setVisObject(visObjectFactory->makeObject(
						primitiveFactory->makeTruncatedCone(r,root->getNumDescendants() > 1,
						topClosed)));
			} else {
				visObjectFactory->updateObjectMatrix(root->getVisObject());
			}
		}
		glPopMatrix();
	}
}

void Visualizer::traverseTreeT(Cluster *root,int rot) {
	if (root == lts->getInitialState()->getCluster()) {
		glPushMatrix();
			glRotatef(180.0f,0.0f,1.0f,0.0f);
			glScalef(root->getTopRadius(),root->getTopRadius(),
							 min(root->getTopRadius(),settings->getFloat(ClusterHeight)));
			if (create_objects) {
				root->setVisObjectTop(visObjectFactory->makeObject(
													 primitiveFactory->makeHemisphere()));
			} else {
				visObjectFactory->updateObjectMatrix(root->getVisObjectTop());
			}
		glPopMatrix();
	}
	if (!root->hasDescendants()) {
		glPushMatrix();
			glScalef(root->getTopRadius(),root->getTopRadius(),
							 min(root->getTopRadius(),settings->getFloat(ClusterHeight)));
			if (create_objects) {
				root->setVisObject(visObjectFactory->makeObject(
													 primitiveFactory->makeHemisphere()));
			} else {
				visObjectFactory->updateObjectMatrix(root->getVisObject());
			}
		glPopMatrix();
	}
	else {
		int drot = rot + settings->getInt(BranchRotation);
		if (drot >= 360) {
			drot -= 360;
		}
		
		float baserad = 0.0f;
		for (int i = 0; i < root->getNumDescendants(); ++i) {
			Cluster* desc = root->getDescendant(i);
			if (desc->getPosition() < -0.9f) {
				baserad = desc->getTopRadius();
				glTranslatef(0.0f,0.0f,settings->getFloat(ClusterHeight));
				if (root->getNumDescendants() > 1) {
					traverseTreeT(desc,drot);
				} else {
					traverseTreeT(desc,rot);
				}
				glTranslatef(0.0f,0.0f,-settings->getFloat(ClusterHeight));
			} else {
				glRotatef(-desc->getPosition()-rot,0.0f,0.0f,1.0f);
				
				// make the connecting cone
				float sz = sqrt(settings->getFloat(ClusterHeight) *
						settings->getFloat(ClusterHeight) +
						(root->getBaseRadius() - root->getTopRadius()) *
						(root->getBaseRadius() - root->getTopRadius()));
				float alpha = atan(settings->getFloat(ClusterHeight) /
											fabs(root->getBaseRadius() - root->getTopRadius()));
				float sign = 1.0f;
				if (root->getBaseRadius() - root->getTopRadius() < 0.0f) {
					sign = -1.0f;
				}
				glPushMatrix();
					glTranslatef(root->getTopRadius(),0.0f,0.0f);
					glRotatef(sign*(90.0f-rad_to_deg(alpha)),0.0f,1.0f,0.0f);
					glScalef(sz,sz,sz);
					if (create_objects) {
						desc->setVisObjectTop(visObjectFactory->makeObject(
								primitiveFactory->makeObliqueCone(alpha,
								desc->getTopRadius()/sz,sign)));
					} else {
						visObjectFactory->updateObjectMatrix(desc->getVisObjectTop());
					}
				glPopMatrix();
				
				// recurse into the subtree
				glTranslatef(root->getBaseRadius(),0.0f,
										 settings->getFloat(ClusterHeight));
				glRotatef(settings->getInt(BranchTilt),0.0f,1.0f,0.0f);
				traverseTreeT(desc,drot);
				glRotatef(-settings->getInt(BranchTilt),0.0f,1.0f,0.0f);
				glTranslatef(-root->getBaseRadius(),0.0f,
											-settings->getFloat(ClusterHeight));
				glRotatef(desc->getPosition()+rot,0.0f,0.0f,1.0f);
			}
		}
		
		if (baserad <= 0.0f) {
			// root has no centered descendant, so draw it as a hemispheroid
			glPushMatrix();
				glScalef(root->getTopRadius(),root->getTopRadius(),
								 min(root->getTopRadius(),settings->getFloat(ClusterHeight)));
				if (create_objects) {
					root->setVisObject(visObjectFactory->makeObject(
														 primitiveFactory->makeHemisphere()));
				} else {
					visObjectFactory->updateObjectMatrix(root->getVisObject());
				}
			glPopMatrix();
		} else {
			float r = baserad / root->getTopRadius();
			glPushMatrix();
			glTranslatef(0.0f,0.0f,0.5f*settings->getFloat(ClusterHeight));
			if (r > 1.0f) {
				r = 1.0f / r;
				glRotatef(180.0f,1.0f,0.0f,0.0f);
				glScalef(baserad,baserad,settings->getFloat(ClusterHeight));
				if (create_objects) {
					root->setVisObject(visObjectFactory->makeObject(
							primitiveFactory->makeTruncatedCone(r,false,false)));
				} else {
					visObjectFactory->updateObjectMatrix(root->getVisObject());
				}
			} else {
				glScalef(root->getTopRadius(),root->getTopRadius(),
								 settings->getFloat(ClusterHeight));
				if (create_objects) {
					root->setVisObject(visObjectFactory->makeObject(
							primitiveFactory->makeTruncatedCone(r,false,false)));
				} else {
					visObjectFactory->updateObjectMatrix(root->getVisObject());
				}
			}
			glPopMatrix();
		}
	}
}

float Visualizer::compute_cone_scale_x(float phi,float r,float x) {
	float f = r/x * sin(phi);
	return r * cos(phi) / sqrt(1.0f - f*f);
}

void Visualizer::updateColors() {
	int r,i;
	RGB_Color c;
	if (markStyle == NO_MARKS) {
		Interpolater ipr(settings->getRGB(InterpolateColor1),
				settings->getRGB(InterpolateColor2),lts->getNumRanks(),
				settings->getBool(LongInterpolation));
		for (r = 0; r < lts->getNumRanks(); ++r) {
			c = ipr.getColor(r);
			for (i = 0; i < lts->getNumClustersAtRank(r); ++i) {
				// set color of cluster[r,i]
				visObjectFactory->updateObjectColor(
						lts->getClusterAtRank(r,i)->getVisObject(),c);
			}
		}
	} else {
		c = settings->getRGB(MarkedColor);
		for (r = 0; r < lts->getNumRanks(); ++r) {
			for (i = 0; i < lts->getNumClustersAtRank(r); ++i) {
				// set color of cluster[r,i]
				if (isMarked(lts->getClusterAtRank(r,i))) {
					visObjectFactory->updateObjectColor(
							lts->getClusterAtRank(r,i)->getVisObject(),c);
				} else {
					visObjectFactory->updateObjectColor(
							lts->getClusterAtRank(r,i)->getVisObject(),RGB_WHITE);
				}
			}
		}
	}
}

void Visualizer::sortClusters(Point3D viewpoint) {
	visObjectFactory->sortObjects(viewpoint);
}

bool Visualizer::isMarked(Cluster* c) {
	return ((markStyle == MARK_STATES && c->hasMarkedState()) ||
					(markStyle == MARK_DEADLOCKS && c->hasDeadlock()) ||
					(markStyle == MARK_TRANSITIONS && c->hasMarkedTransition()));
}

// ------------- STATES --------------------------------------------------------

void Visualizer::drawStates() {
  if (lts == NULL) {
  	return;
  }
  drawStates(lts->getInitialState()->getCluster(),0);
}

void Visualizer::drawSimStates(vector<State*> historicStates, 
                               State* currState) {
  if (lts == NULL) {
    return;
  }
  // Compute absolute positions of nodes
  Point3D init = {0, 0, 0};
  glPushMatrix();
    glLoadIdentity();
    glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
    computeStateAbsPos(lts->getInitialState(), 0, init);
  glPopMatrix();
  
  // We want to see marked states during simulation, so draw these first
  Cluster* root = lts->getInitialState()->getCluster();
  drawSimMarkedStates(root, 0);
    
  // Draw previous states of the simulation, in the colour specified in the
  // settings (default: white)
  float ns = settings->getFloat(NodeSize);
  RGB_Color hisStateColor = settings->getRGB(SimStateColor);
  glColor4ub(hisStateColor.r, hisStateColor.g, hisStateColor.b, 255);
  
  for (size_t i = 0; i < historicStates.size() - 1; ++i) {
    State* s = historicStates[i];
    if (!isMarked(s)) {
      Point3D p = s->getPositionAbs();
      glPushMatrix(); 
      glTranslatef(p.x, p.y, p.z);
      glScalef(ns, ns, ns);
      primitiveFactory->drawSimpleSphere();
      glPopMatrix();
    }
  }
 
  // Draw the current state.
  //RGB_Color hisStateColor = settings->getRGB(SimStateColor);
  RGB_Color currStateColor = settings->getRGB(SimCurrStateColor);
  glColor4ub(currStateColor.r, currStateColor.g, currStateColor.b, 255);
  Point3D currentPos = currState->getPositionAbs();
  glPushMatrix();
  glTranslatef(currentPos.x, currentPos.y, currentPos.z);

  // Make the current state a bit larger, to make it easier to find it in the
  // simulation
  glScalef(1.5 *ns, 1.5* ns, 1.5 * ns);
  primitiveFactory->drawSimpleSphere();
  glPopMatrix();

}

void Visualizer::drawSimMarkedStates(Cluster* root, int rot) {
  float ns = settings->getFloat(NodeSize);

  for (int i = 0; i < root->getNumStates(); ++i) {
    State* s = root->getState(i);
    
    if(isMarked(s)) {
      RGB_Color c = settings->getRGB(MarkedColor);
      glColor4ub(c.r, c.g, c.b, 255);
      
      glPushMatrix();
      if (s->getPosition() >= -0.9f) {
        glRotatef(-s->getPosition(), 0.0f, 0.0f, 1.0f);
        glTranslatef(root->getTopRadius(), 0.0f, 0.0f);
      }
      glScalef(ns, ns, ns);
      primitiveFactory->drawSimpleSphere();
      glPopMatrix();
    }
  }

  int drot = rot + settings->getInt(BranchRotation);
  if (drot >= 360) {
    drot -= 360;
  }

  Cluster* desc;

  for (int i = 0; i < root->getNumDescendants(); ++i) {
    desc = root->getDescendant(i);
    if (desc->getPosition() < -0.9f){
      // Descendant is centered
      glTranslatef(0.0f, 0.0f, settings->getFloat(ClusterHeight));
      drawSimMarkedStates(desc, (root->getNumDescendants() > 1)?drot:rot);
      glTranslatef(0.0f, 0.0f, -settings->getFloat(ClusterHeight));
    }
    else {
      glRotatef(-desc->getPosition() - rot, 0.0f, 0.0f, 1.0f);
      glTranslatef(root->getBaseRadius(), 0.0f,
                   settings->getFloat(ClusterHeight));
      glRotatef(settings->getInt(BranchTilt), 0.0f, 1.0f, 0.0f);
      drawSimMarkedStates(desc, drot);
      glRotatef(-settings->getInt(BranchTilt), 0.0f, 1.0f, 0.0f);
      glTranslatef(-root->getBaseRadius(), 0.0f, 
                   -settings->getFloat(ClusterHeight));
      glRotatef(desc->getPosition() + rot, 0.0f, 0.0f, 1.0f);
    }
  }
}

void Visualizer::clearDFSStates(State* root) {
  root->DFSclear();
  for(int i=0; i!=root->getNumOutTransitions(); ++i) {
    Transition* outTransition = root->getOutTransitioni(i);
    if (!outTransition->isBackpointer()) {
      State* endState = outTransition->getEndState();
      if (endState->getVisitState() != DFS_WHITE) {
        clearDFSStates(endState);
      }
    }
  }
}

void Visualizer::computeStateAbsPos(State* root, int rot, Point3D initVect) {
// Does a DFS on the states to calculate their `absolute' position, taking the 
// position of the initial state as (0,0,0). 
// Secondly, it also assigns the incoming and outgoing control points for the 
// back pointers of each state. 
  root->DFSvisit();
  Cluster* startCluster = root->getCluster();

  float M[16];
  float N[16]; // For the backpointers
  glGetFloatv(GL_MODELVIEW_MATRIX, M);

  if (root->getRank() == 0) {
    // Root is the initial state of the system.
    Point3D initPos = {0, 0, 0};
    root->setPositionAbs(initPos);
    initVect.x = M[12];
    initVect.y = M[14];
    initVect.z = M[13];
  }

  if(root->getPosition() < -0.9f) {
    // The outgoing vector of the state lies settings->getFloat(ClusterHeight) above the state.
    glTranslatef(0.0f, 0.0f, 2 * settings->getFloat(ClusterHeight));
    glGetFloatv(GL_MODELVIEW_MATRIX, N);
    Point3D outgoingVect = { N[12] - initVect.x,
                             N[14] - initVect.y,
                           - N[13] + initVect.z };
    root->setOutgoingControl( outgoingVect );
    glTranslatef(0.0f, 0.0f, -2 * settings->getFloat(ClusterHeight));

    Point3D rootPos = { M[12] - initVect.x, 
                        M[14] - initVect.y, 
                      - M[13] + initVect.z};    
    root->setPositionAbs(rootPos);

    // The incoming vector of the state lies settings->getFloat(ClusterHeight) beneath the state.
    glTranslatef(0.0f, 0.0f, -2 * settings->getFloat(ClusterHeight));
    glGetFloatv(GL_MODELVIEW_MATRIX, N);
    Point3D incomingVect = { N[12] - initVect.x,
                             N[14] - initVect.y,
                           - N[13] + initVect.z };
    root->setIncomingControl( incomingVect );
    glTranslatef(0.0f, 0.0f, 2 * settings->getFloat(ClusterHeight));
  }
  else {
    // The outgoing vector of the state points out of the cluster, in the 
    // direction the state itself is positioned. Furhtermore, it points 
    // settings->getFloat(ClusterHeight) up.
    glRotatef(-root->getPosition(), 0.0f, 0.0f, 1.0f);

    glTranslatef(startCluster->getTopRadius() * 3, 0.0f, -settings->getFloat(ClusterHeight));
    glGetFloatv(GL_MODELVIEW_MATRIX, N);
    Point3D outgoingVect = { N[12] - initVect.x,
                             N[14] - initVect.y,
                            -N[13] + initVect.z };
    root->setOutgoingControl(outgoingVect);
    glTranslatef(-startCluster->getTopRadius() * 2, 0.0f, settings->getFloat(ClusterHeight));
    
    glGetFloatv(GL_MODELVIEW_MATRIX, M);
    Point3D rootPos = { M[12] - initVect.x, 
                        M[14] - initVect.y, 
                      - M[13] + initVect.z};                    
    root->setPositionAbs(rootPos);

    glTranslatef( startCluster->getTopRadius() * 2, 0.0f, settings->getFloat(ClusterHeight));
    glGetFloatv(GL_MODELVIEW_MATRIX, N);
    Point3D incomingVect = { N[12] - initVect.x,
                             N[14] - initVect.y,
                            -N[13] + initVect.z };
    root->setIncomingControl(incomingVect);
    glTranslatef(-startCluster->getTopRadius() * 3, 0.0f, -settings->getFloat(ClusterHeight));
    glRotatef(root->getPosition(), 0.0f, 0.0f, 1.0f);
  }

  for(int i = 0; i != root->getNumOutTransitions(); ++i) {
    Transition* outTransition = root->getOutTransitioni(i);
    State* endState = outTransition->getEndState();

    if (endState->getVisitState() == DFS_WHITE &&
        !outTransition->isBackpointer()) {

      int drot = rot + settings->getInt(BranchRotation);
      if (drot < 0) {
        drot += 360;
      }
      else if (drot >= 360) {
        drot -=360;
      }

      Cluster* endCluster = endState->getCluster();

      if (endState->getRank() != root->getRank()) {
        
        if (endCluster->getPosition() < -0.9f) {
          //endCluster is centered, only descend
          glTranslatef(0.0f, 0.0f, settings->getFloat(ClusterHeight));
          computeStateAbsPos(endState, 
            (startCluster->getNumDescendants()>1)?drot: rot,
            initVect);
          glTranslatef(0.0f, 0.0f, -settings->getFloat(ClusterHeight));
        }
        else {
          glRotatef(-endCluster->getPosition() - rot, 0.0f, 0.0f, 1.0f);
          glTranslatef(startCluster->getBaseRadius(), 0.0f, settings->getFloat(ClusterHeight));
          glRotatef(settings->getInt(BranchTilt), 0.0f, 1.0f, 0.0f);
          computeStateAbsPos(endState, drot, initVect);
          glRotatef(-settings->getInt(BranchTilt), 0.0f, 1.0f, 0.0f);
          glTranslatef(-startCluster->getBaseRadius(), 0.0f, -settings->getFloat(ClusterHeight));
          glRotatef(endCluster->getPosition() + rot, 0.0f, 0.0f, 1.0f);
        }
      }
    }
  }
  // Finalize this node
  root->DFSfinish();
}

void Visualizer::drawStates(Cluster* root,int rot) {
  float ns = settings->getFloat(NodeSize);
  for (int i = 0; i < root->getNumStates(); ++i) {
    State *s = root->getState(i);
    if (isMarked(s)) {
      RGB_Color c = settings->getRGB(MarkedColor);
      glColor4ub(c.r,c.g,c.b,255);
    } 
    else {
      RGB_Color c = settings->getRGB(StateColor);
      glColor4ub(c.r,c.g,c.b,255);
    }
    glPushMatrix();
    if (s->getPosition() >= -0.9f) {
      glRotatef(-s->getPosition(),0.0f,0.0f,1.0f);
      glTranslatef(root->getTopRadius(),0.0f,0.0f);  
    }
    glScalef(ns,ns,ns);
    primitiveFactory->drawSimpleSphere();
    glPopMatrix();
  }

  int drot = rot + settings->getInt(BranchRotation);
	if (drot >= 360) { 
		drot -= 360;
	}
  Cluster *desc;
  for (int i = 0; i < root->getNumDescendants(); ++i) {
    desc = root->getDescendant(i);
    if (desc->getPosition() < -0.9f) {
      // descendant is centered
      glTranslatef(0.0f,0.0f,settings->getFloat(ClusterHeight));
      drawStates(desc,(root->getNumDescendants()>1)?drot:rot);
      glTranslatef(0.0f,0.0f,-settings->getFloat(ClusterHeight));
    } else {
      glRotatef(-desc->getPosition()-rot,0.0f,0.0f,1.0f);
      glTranslatef(root->getBaseRadius(),0.0f,settings->getFloat(ClusterHeight));
      glRotatef(settings->getInt(BranchTilt), 0.0f, 1.0f, 0.0f);
      drawStates(desc,drot);
      glRotatef(-settings->getInt(BranchTilt), 0.0f, 1.0f, 0.0f);
      glTranslatef(-root->getBaseRadius(),0.0f,-settings->getFloat(ClusterHeight));
      glRotatef(desc->getPosition()+rot,0.0f,0.0f,1.0f);
    }
  }
}

bool Visualizer::isMarked(State* s) {
  return ((markStyle == MARK_STATES && s->isMarked()) || 
          (markStyle == MARK_DEADLOCKS && s->isDeadlock()));
}

// ------------- TRANSITIONS ---------------------------------------------------

void Visualizer::drawTransitions(bool draw_fp,bool draw_bp) {
  if (lts == NULL) return;
  if (!draw_fp && !draw_bp) return;

  clearDFSStates(lts->getInitialState());
  Point3D init = {0, 0, 0};
  glPushMatrix();
    glLoadIdentity();
    glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
    computeStateAbsPos(lts->getInitialState(), 0, init);
  glPopMatrix();
  clearDFSStates(lts->getInitialState());
        
  drawTransitions(lts->getInitialState(),draw_fp,draw_bp);
}

void Visualizer::drawTransitions(State* root,bool disp_fp,bool disp_bp) {
  root->DFSvisit();

  for (int i = 0; i != root->getNumOutTransitions(); ++i) {
    Transition* outTransition = root->getOutTransitioni(i);

    State* endState = outTransition->getEndState();

    // Draw transition from root to endState
    if (outTransition->isBackpointer() && disp_bp) {
    	if (isMarked(outTransition)) {
    		RGB_Color c = settings->getRGB(MarkedColor);
      	glColor4ub(c.r,c.g,c.b,255);
    	} else {
    		RGB_Color c = settings->getRGB(UpEdgeColor);
      	glColor4ub(c.r,c.g,c.b,255);
    	}
      drawBackPointer(root,endState);
    }
    if (!outTransition->isBackpointer() && disp_fp) {
    	if (isMarked(outTransition)) {
    		RGB_Color c = settings->getRGB(MarkedColor);
      	glColor4ub(c.r,c.g,c.b,255);
    	} else {
    		RGB_Color c = settings->getRGB(DownEdgeColor);
      	glColor4ub(c.r,c.g,c.b,255);
    	}
      drawForwardPointer(root,endState);
    }
    
    // If we haven't visited endState before, do so now.
    if (endState->getVisitState() == DFS_WHITE && 
        !outTransition->isBackpointer()) {
      // Move to the next state
      drawTransitions(endState,disp_fp,disp_bp);
    }
  }
  // Finalize this node
  root->DFSfinish();
}

void Visualizer::drawSimTransitions(bool draw_fp, bool draw_bp, 
                                 vector<Transition*> transHis, 
                                 vector<Transition*> posTrans,
                                 Transition* chosenTrans) {
  float ns = settings->getFloat(NodeSize);
  
  // Draw the historical transitions, in orange for the moment.
  for (size_t i = 0; i < transHis.size(); ++i) {
    Transition* currTrans = transHis[i];
    State* beginState = currTrans->getBeginState();
    State* endState = currTrans->getEndState();

    // Draw transition from beginState to endState
    if (currTrans->isBackpointer() && draw_bp) {
      if (isMarked(currTrans)) {
        RGB_Color c = settings->getRGB(MarkedColor);
        glColor4ub(c.r, c.g, c.b, 255);
      }
      else {
        RGB_Color transColor = settings->getRGB(SimUpEdgeColor);
        glColor4ub(transColor.r, transColor.g, transColor.b, 255);
      }
      drawBackPointer(beginState, endState);
    }
    if (!currTrans->isBackpointer() && draw_fp) {
      if (isMarked(currTrans)) {
        RGB_Color c = settings->getRGB(MarkedColor);
        glColor4ub(c.r, c.g, c.b, 255);
      }
      else {
        RGB_Color transColor = settings->getRGB(SimDownEdgeColor);
        glColor4ub(transColor.r, transColor.g, transColor.b, 255);
      }
      drawForwardPointer(beginState, endState);
    }
  }

  // Draw the possible transitions from the current state, as well as the state 
  // they lead into
  for (size_t i = 0; i < posTrans.size(); ++i) {
    Transition* currTrans = posTrans[i];
    State* beginState = currTrans->getBeginState();
    State* endState = currTrans->getEndState();
    
    
    // Draw transition from beginState to endState
    if (currTrans->isBackpointer() && draw_bp) {
      if (currTrans == chosenTrans) {
        RGB_Color c = settings->getRGB(SimSelEdgeColor);
        glColor4ub(c.r, c.g, c.b, 255);
        glLineWidth(2.0);
      }
      else {
        RGB_Color c = settings->getRGB(SimPosEdgeColor);
        glColor4ub(c.r, c.g, c.b, 255);
      }
      if (isMarked(currTrans)) {
        RGB_Color c = settings->getRGB(MarkedColor);
        glColor4ub(c.r, c.g, c.b, 255);
      }
      drawBackPointer(beginState, endState);
      glLineWidth(1.0);
    }
    if (!currTrans->isBackpointer() && draw_fp) {
      if (currTrans == chosenTrans) {
        RGB_Color c  = settings->getRGB(SimSelEdgeColor);
        glColor4ub(c.r, c.g, c.b, 255);
        glLineWidth(2.0);
      }
      else {
        RGB_Color c = settings->getRGB(SimPosEdgeColor);
        glColor4ub(c.r, c.g, c.b, 255);
      }
      if (isMarked(currTrans)) {
        RGB_Color c = settings->getRGB(MarkedColor);
        glColor4ub(c.r, c.g, c.b, 255);
      }
      drawForwardPointer(beginState, endState);
      glLineWidth(1.0);
    }
    
    if (!isMarked(endState)) {
      if (currTrans != chosenTrans) {
        RGB_Color c = settings->getRGB(SimPosStateColor);
        glColor4ub(c.r, c.g, c.b, 255);
        Point3D p = endState->getPositionAbs();
        glPushMatrix();
        glTranslatef(p.x, p.y, p.z);
        glScalef(ns, ns, ns);
        primitiveFactory->drawSimpleSphere();
        glPopMatrix();
      }
      else {
        // Draw the chosen state bigger and in another colour.
        RGB_Color c = settings->getRGB(SimSelStateColor);
        glColor4ub(c.r, c.g, c.b, 255);
        Point3D p = endState->getPositionAbs();
        glPushMatrix();
        glTranslatef(p.x, p.y, p.z);
        glScalef(1.5 * ns, 1.5 * ns, 1.5* ns);
        primitiveFactory->drawSimpleSphere();
        glPopMatrix();
      }
    }
  }
}
  
void Visualizer::drawForwardPointer(State* startState, State* endState) {
  Point3D startPoint = startState->getPositionAbs();
  Point3D endPoint = endState->getPositionAbs();

  glBegin(GL_LINES);
    glVertex3f(startPoint.x, startPoint.y, startPoint.z);
    glVertex3f(endPoint.x, endPoint.y, endPoint.z);
  glEnd();
}

void Visualizer::drawBackPointer(State* startState, State* endState) {
  Point3D startPoint = startState->getPositionAbs();
  Point3D startControl = startState->getOutgoingControl();
  Point3D endControl = endState->getIncomingControl();
  Point3D endPoint = endState->getPositionAbs();

  if ((startState->getPosition() < -0.9f) && (endState->getPosition() < -0.9f)) 
  {
    startControl.x = startPoint.x * 1.25;
    endControl.x = startControl.x;
  }

  GLfloat ctrlPts [4][3] = { {startPoint.x, startPoint.y, startPoint.z},
                             {startControl.x, startControl.y, startControl.z},
                             {endControl.x, endControl.y, endControl.z},
                             {endPoint.x, endPoint.y, endPoint.z} };
  float t,it,b0,b1,b2,b3,x,y,z;       
  glBegin(GL_LINE_STRIP);
    for (GLint k = 0; k < 50; ++k) {
      t  = (float)k / 49;
      it = 1.0f - t;
      b0 =      t *  t *  t;
      b1 = 3 *  t *  t * it;
      b2 = 3 *  t * it * it;
      b3 =     it * it * it;

      x = b0 * ctrlPts[0][0] +
          b1 * ctrlPts[1][0] + 
          b2 * ctrlPts[2][0] +
          b3 * ctrlPts[3][0];

      y = b0 * ctrlPts[0][1] +
          b1 * ctrlPts[1][1] +
          b2 * ctrlPts[2][1] +
          b3 * ctrlPts[3][1];
                
      z = b0 * ctrlPts[0][2] +
          b1 * ctrlPts[1][2] +
          b2 * ctrlPts[2][2] +
          b3 * ctrlPts[3][2];
      glVertex3f(x,y,z);
    }
  glEnd();
}

bool Visualizer::isMarked(Transition* t) {
  return markStyle == MARK_TRANSITIONS && t->isMarked();
}
