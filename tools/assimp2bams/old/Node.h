#ifndef Node_H
#define Node_H

class Node{
public:
	vec3 mTranslation;
	quat mRotation;
	
	std::vector<Node*> mChild;
	
	Node(){
		mTranslation = vec3(0,0,0);
		mRotation = quat(0,0,0,1);
	}

};

#endif
