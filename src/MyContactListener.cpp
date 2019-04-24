#pragma once
#include "MyContactListener.hpp"


	void MyContactListener::BeginContact(b2Contact * contact)
	{

		int Aentitytype = -1;
		int Bentitytype = -1;
		Renderable* object1;
		Renderable* object2;

		//check if fixture A type
		void* bodyUserData = contact->GetFixtureA()->GetBody()->GetUserData();
		if (bodyUserData) {
			object1 = static_cast<Renderable*>(bodyUserData);
			Aentitytype = static_cast<Renderable*>(bodyUserData)->getEntityType();
		}

		//check if fixture B type
		bodyUserData = contact->GetFixtureB()->GetBody()->GetUserData();
		if (bodyUserData) {
			object2 = static_cast<Renderable*>(bodyUserData);
			Bentitytype = static_cast<Renderable*>(bodyUserData)->getEntityType();
		}


		if (Aentitytype != -1 && Bentitytype != -1) {
		
			//call functions inside each class
			//half idea: making a function called handleCollision(Renderable* obj1, Renderable* obj2, int obj1entity, int obj2entity) in renderable struct,
			//implement this function in each class instead of detecting all types in this listener
			object1->handleCollision(object1, object2, Aentitytype, Bentitytype);

		}

	}

	void MyContactListener::EndContact(b2Contact * contact)
	{

	}
