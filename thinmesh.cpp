/* 
 * This file is part of the Thinmesh distribution (https://github.com/wredenberg/thinmesh).
 * Copyright (c) 2007 Fredrik Wredenberg.
 * 
 * This program is free software: you can redistribute it and/or modify  
 * it under the terms of the GNU General Public License as published by  
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but 
 * WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License 
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */


#include <iostream>
#include <vector>
#include <string.h>
#include <stdio.h>
using namespace std;

bool partOf(vector<int> A, vector<int> B); //is the domain A a part of domain B?
bool intersect(vector<int> A, vector<int> B); //does domain A intersect domain B?
bool partOfSpecial(vector<int> A, vector<int> B, int number); //is the domain A a part of domain B (with "number" elements)?
vector<int> getIntersection(vector<int> A, vector<int> B); //returns all common elements of A and B
vector<int> subtract(vector<int> A, vector<int> B); //returns elements of A - B
vector<int> add(vector<int> A, vector<int> B); //returns elements of A + B

struct coord
{
	double x;
	double y;
	double z;
	
	coord()
	{
		x=0;
		y=0;
		z=0;
	}
};

struct refPoint
{
	double x;
	double z;
	int type; //0 if division y wize and 1 if z wise
	
	refPoint()
	{
		x=0;
		z=0;
		type=-1;
	}
};

struct node
{
	int nNum;
	double x;
	double y;
	double z;
	int sectorNumber;
	
	node()
	{
		nNum=-1;
		x=-1;
		y=-1;
		z=-1;
		sectorNumber=-1;
	}
};

struct sector
{
	int num;
	vector<int> inhabitants; //nodes in the sector (only the numbers)
	vector<int> neighbors; //neighboring sectors
	sector()
	{
		num=-1;
	}
	void addNode(int nnum)
	{
		inhabitants.push_back(nnum);
	}
	void removeNode(int nnum)
	{
		
	}
	vector<int> getInhabitants()
	{
		return inhabitants;
	}
	vector<int> getNeighbors()
	{
		return neighbors;
	}
};


struct nodeBox //contains a vector of nodes
{
	vector<node> nodes;
	vector<int> map; //map to the nodes. contains the positions of elements in elementList;
	
	nodeBox()
	{
		printf("Nodebox created\n");
	}
	
	int size()
	{
		return map.size();
	}
	
	int addNode(node nodeToAdd)
	{
		int nNum=nodeToAdd.nNum;
		if((int)map.size()<=nNum)
		{
			map.resize(nNum+1,-1);
		}
		else
		{
			printf("ERROR! Added a node that already exists. Use replace instead. (Node %d) \n", nNum);
			return -1;	//failure
		}
		nodes.push_back(nodeToAdd);
		int pos=nodes.size()-1;
		map.at(nNum)=pos;
		
		//test if the node actually is where its suppused to be
//		node testNode;
//		testNode=getNode(nNum);
//		if(testNode.nNum!=nNum)
//		{
//			printf("ERROR: Messed up the node map while adding a node\n");
//		}
		//checkMap();
		
		return 1; //sucess
	}
	
	node getNode(int nNum)
	{
		reMap();
		if(nNum>=(int) map.size())
		{
			printf("No such node\n");
		}
		//printf("Trying to get node no. %d",nNum);
		int pos=map.at(nNum);
		//printf(", which is supposed to be in pos %d.\n",pos);
		node toBeReturned;
		toBeReturned=nodes.at(pos);
		//checkMap();
		
		if(toBeReturned.nNum!=nNum)
		{
			printf("Corrupt map in getNode\n");
		}
		return toBeReturned;
	}
	
	vector<int> getNodeNumbers() //returns the node numbers
	{
		vector<int> toBeReturned;
		for(unsigned int i=0; i<nodes.size(); i++) // goes through all nodes
		{
			toBeReturned.push_back(nodes.at(i).nNum);
		}
		return toBeReturned;
	}
	vector<int> getNodesAt(double x, double y, double z, double tol) //returns the node numbers at a specific coordinate
	{
		vector<int> toBeReturned;
		for(unsigned int i=0; i<nodes.size(); i++) // goes through all nodes
		{
			if((x<=nodes.at(i).x+tol && x>=nodes.at(i).x-tol ) && (y<=nodes.at(i).y+tol && y>=nodes.at(i).y-tol ) && (z<=nodes.at(i).z+tol && z>=nodes.at(i).z-tol))
			{
				toBeReturned.push_back(nodes.at(i).nNum);
			}
		}
		return toBeReturned;
	}
	
	vector<int> nselX(double x_low, double x_high, double tol) //returns the node numbers at a specific coordinate
	{
		vector<int> toBeReturned;
		for(unsigned int i=0; i<nodes.size(); i++) // goes through all nodes
		{
			if((x_high+tol>=nodes.at(i).x && x_low-tol<=nodes.at(i).x))
			{
				toBeReturned.push_back(nodes.at(i).nNum);
			}
		}
		return toBeReturned;
	}
	
	vector<int> nselY(double y_low,double y_high, double tol) //returns the node numbers at a specific coordinate
	{
		vector<int> toBeReturned;
		for(unsigned int i=0; i<nodes.size(); i++) // goes through all nodes
		{
			if((y_high+tol>=nodes.at(i).y && y_low-tol<=nodes.at(i).y))
			{
				toBeReturned.push_back(nodes.at(i).nNum);
			}
		}
		return toBeReturned;
	}
	
	vector<int> nselZ(double z, double tol) //returns the node numbers at a specific coordinate
	{
		vector<int> toBeReturned;
		for(unsigned int i=0; i<nodes.size(); i++) // goes through all nodes
		{
			if((z<=nodes.at(i).z+tol && z>=nodes.at(i).z-tol ))
			{
				toBeReturned.push_back(nodes.at(i).nNum);
			}
		}
		return toBeReturned;
	}
	
	vector<int> nselXYZ(double xl,double xh,double yl,double yh,double zl,double zh) //returns the node numbers at a specific coordinate
	{
		vector<int> toBeReturned;
		for(unsigned int i=0; i<nodes.size(); i++) // goes through all nodes
		{
			if((xh>=nodes.at(i).x && xl<=nodes.at(i).x) && (yh>=nodes.at(i).y && yl<=nodes.at(i).y) && (zh>=nodes.at(i).z && zl<=nodes.at(i).z))
			{
				toBeReturned.push_back(nodes.at(i).nNum);
			}
		}
		return toBeReturned;
	}

	vector<int> nselXYZinverted(double xl,double xh,double yl,double yh,double zl,double zh) //returns the node numbers at a specific coordinate
	{
		vector<int> toBeReturned;
		for(unsigned int i=0; i<nodes.size(); i++) // goes through all nodes
		{
			if((xh<=nodes.at(i).x || xl>=nodes.at(i).x) || (yh<=nodes.at(i).y || yl>=nodes.at(i).y) || (zh<=nodes.at(i).z || zl>=nodes.at(i).z))
			{
				toBeReturned.push_back(nodes.at(i).nNum);
			}
		}
		return toBeReturned;
	}

	vector<int> nselXYZam(double xl,double xh,double yl,double yh,double zl,double zh) //returns the node numbers at a specific coordinate
	{
		vector<int> toBeReturned;
		for(unsigned int i=0; i<nodes.size(); i++) // goes through all nodes
		{
			if(((xh<=nodes.at(i).x || xl>=nodes.at(i).x) || (zh<=nodes.at(i).z || zl>=nodes.at(i).z)) && (yh>=nodes.at(i).y && yl<=nodes.at(i).y))
			{
				toBeReturned.push_back(nodes.at(i).nNum);
			}
		}
		return toBeReturned;
	}
	
	
	vector<int> getNodesToTheLeftOf(double x) //returns the node numbers of nodes to the left of x (<x)
	{
		vector<int> toBeReturned;
		for(unsigned int i=0; i<nodes.size(); i++) // goes through all nodes
		{
			if(x>nodes.at(i).x)
			{
				toBeReturned.push_back(nodes.at(i).nNum);
			}
		}
		return toBeReturned;
	}
	
	vector<int> getNodesToTheRightOf(double x) //returns the node numbers of nodes to the left of x (<x)
	{
		vector<int> toBeReturned;
		for(unsigned int i=0; i<nodes.size(); i++) // goes through all nodes
		{
			if(x<nodes.at(i).x)
			{
				toBeReturned.push_back(nodes.at(i).nNum);
//				printf("node to the right: %d\n",nodes.at(i).nNum);
//				printf("node location: %f x=%f\n",nodes.at(i).x,x);
			}
		}
		return toBeReturned;
	}
	
	vector<node> getNodes()
	{
		return nodes;
	}
	
	void deleteNode(int toDelete)
	{
		int pos=map.at(toDelete);
		int oldSize=nodes.size();
		for(int i=pos;i<oldSize-1;i++) // move all nodes after the removed one down one step
		{
			nodes.at(i)=nodes.at(i+1);
		}
		nodes.pop_back(); // deletes the last node
		reMap();
		//checkMap();
	}
	
	void reMap() // recreates the node map
	{
		//printf("Remapping\n");
		int highestNodeNumber=-1;
		for(unsigned int i=0;i<nodes.size();i++) //find the largest node number
		{
			if(nodes.at(i).nNum>highestNodeNumber)
			{
				highestNodeNumber=nodes.at(i).nNum;
			}
		}
		map.resize(highestNodeNumber+1,-1);
		for(unsigned int i=0;i<nodes.size();i++) //go through all nodes and place a location on the map
		{
			int nNum=nodes.at(i).nNum;
			map.at(nNum)=i;
		}
		//checkMap();
	}
	
	int checkMap() // check map integrity
	{
		//1 = map is ok, 0 = map is corrupted
		for(int i=0;i<(int) nodes.size();i++)
		{
			int nNum=nodes.at(i).nNum;
			int pos=map.at(nNum);
			if(pos!=i)
			{
				printf("Node map is corrupted. (in method checkMap)\n");
				return 0;
			}	
		}
		return 1;
	}
	
	void printNodes()
	{
		printf("Nodes:\n");
		for(unsigned int i=0; i<nodes.size();i++)
		{
			printf("%d,%f,%f,%f\n",nodes.at(i).nNum,nodes.at(i).x,nodes.at(i).y,nodes.at(i).z);
		}
	}
	
	int getHighestNodeNumber()
	{
		int highestNodeNumber=map.size()-1;
//		for(unsigned int i=0;i<nodes.size();i++) //find the largest node number
//		{
//			if(nodes.at(i).nNum>highestNodeNumber)
//			{
//				highestNodeNumber=nodes.at(i).nNum;
//			}
//		}
		return highestNodeNumber;
	}
	
	vector<int> getListOfNodes()
	{
		vector<int> list;
		for(unsigned int i=0; i<nodes.size();i++)
		{
			list.push_back(nodes.at(i).nNum);
		}
		return list;
	}
	
	void shiftNumbers(int number)
	{
		for(unsigned int i=0;i<nodes.size();i++)
		{
			nodes.at(i).nNum=nodes.at(i).nNum+number;
		}
	}
	
	void translateAllNodes(double x,double y,double z)
	{
		for(unsigned int i=0;i<nodes.size();i++)
		{
			nodes.at(i).x=nodes.at(i).x+x;
			nodes.at(i).y=nodes.at(i).y+y;
			nodes.at(i).z=nodes.at(i).z+z;
		}
	}
	
	void translateNode(int nnum,double x,double y,double z)
	{
		int i=map.at(nnum);
		nodes.at(i).x=nodes.at(i).x+x;
		nodes.at(i).y=nodes.at(i).y+y;
		nodes.at(i).z=nodes.at(i).z+z;
	}

	void changeXsign()
	{
		for(unsigned int i=0;i<nodes.size();i++)
		{
			nodes.at(i).x=-nodes.at(i).x;
		}
	}
	
	void changeZsign()
	{
		for(unsigned int i=0;i<nodes.size();i++)
		{
			nodes.at(i).z=-nodes.at(i).z;
		}
	}
	
	void addNodes(vector<node> toAdd)
	{
		for(unsigned int i=0;i<toAdd.size();i++)
		{
			addNode(toAdd.at(i));
		}
	}
};

struct element
{
	int eNum;
	int nodesPerElement=0;
	vector<int> nodes;
	coord cg; //center of gravity
	double wsize;
	
	element()
	{
		eNum=-1;
		wsize=-1;
	}
	
	void setNumber(int i)
	{
		eNum=i;
	}
	
	vector<int> getNodes()
	{
		return nodes;
	}
	
	void setNodes(vector<int> toSet)//, nodeBox nbox)
	{
		nodes=toSet;
		nodesPerElement=nodes.size();
		//calcCenterOfGravity(nbox);
	}
	
	void printNodes()
	{
		printf("Nodes in element no. %d: ",eNum);
		for(int i=0;i<(int)nodes.size();i++)
		{
			printf(" %d",nodes.at(i));
		}
		printf(".\n");
	}
	
	coord getCenterOfGravity() //the arithmetic mean of all nodes
	{
		return cg;
	}
	
	void calcCenterOfGravity(nodeBox nbox)
	{
		double x=0,y=0,z=0;

		for(int i=0;i<nodesPerElement;i++)
		{
			x=x+nbox.getNode(nodes.at(i)).x;
			y=y+nbox.getNode(nodes.at(i)).y;
			z=z+nbox.getNode(nodes.at(i)).z;
		}
		cg.x=x/nodesPerElement;
		cg.y=y/nodesPerElement;
		cg.z=z/nodesPerElement;
		
		//printf("cg %f %f %f \n",cg.x,cg.y,cg.z);
		wsize=nbox.getNode(nodes.at(1)).x-nbox.getNode(nodes.at(0)).x;
		
	}
	
	double getSize()
	{
		return wsize;
	}
	
	void shiftNodeNumbers(int shift)
	{
		for(unsigned int i=0;i<nodes.size();i++)
		{
			nodes.at(i)=nodes.at(i)+shift;
		}
	}

	void mirror()
	{
        if(nodesPerElement==4)
		{
			int n0=nodes.at(0);
			int n1=nodes.at(1);
			int n2=nodes.at(2);
			int n3=nodes.at(3);

			nodes.at(0)=n1;
			nodes.at(1)=n0;
			nodes.at(2)=n3;
			nodes.at(3)=n2;


		}
		else if(nodesPerElement==8)
		{
			int n0=nodes.at(0);
			int n1=nodes.at(1);
			int n2=nodes.at(2);
			int n3=nodes.at(3);
			int n4=nodes.at(4);
			int n5=nodes.at(5);
			int n6=nodes.at(6);
			int n7=nodes.at(7);

			nodes.at(0)=n1;
			nodes.at(1)=n0;
			nodes.at(2)=n3;
			nodes.at(3)=n2;
			nodes.at(4)=n5;
			nodes.at(5)=n4;
			nodes.at(6)=n7;
			nodes.at(7)=n6;
		}
		else
		{
			printf("Error - can not mirror that kind of element! %d\n",nodesPerElement);
		}

	}
	

};

struct elementBox //contains a vector of elements
{
	vector<element> elements;
	vector<int> map; //map to the elements. contains the positions of elements in elementList;
	
	elementBox()
	{
		printf("Element box created\n");
	}
	
	int size()
	{
		return elements.size();
	}
	
	int addElement(element elementToAdd)
	{
//		int eNum=elementToAdd.eNum;
//		map.push_back(eNum);
//		elements.push_back(elementToAdd);
		//printf("adding element %d\n",elementToAdd.eNum);
		
		unsigned int eNum=elementToAdd.eNum;
		if(map.size()<=eNum)
		{
			map.resize(eNum+1,-1);
		}
		else
		{
			printf("ERROR! Added an element that already exists. Use replace instead. (Element %d) \n", eNum);
			return -1;	//failure
		}
		elements.push_back(elementToAdd);
		int pos=elements.size()-1;
		map.at(eNum)=pos;
		
		//printf("In method addElement:\n");
		//elementToAdd.printNodes();
		
		return 1; //sucess
	}
	
	void replaceElement(element toReplace)
	{
		int pos=map.at(toReplace.eNum); //find the position of the element to be replaced
		int eNum=elements.at(pos).eNum;
		if(eNum!=toReplace.eNum) //check if the number of the element to be replaced match the number of the replacement
		{
			printf("Element map corrupted!\n");
		}
		elements.at(pos)=toReplace;
	}
	
	void removeElement(int eNum)
	{
		//todo
	}
	
	element getElement(int eNum)
	{
		//printf("Trying to get element no. %d\n",eNum);
		int pos=map.at(eNum);
		//printf("Which is supposed to be in pos. %d\n",pos);
		element toBeReturned;
		toBeReturned=elements.at(pos);
		if(eNum!=toBeReturned.eNum)
		{
			printf("Element map corrupted!\n");
		}
		//printf("In method get element:\n");
		//toBeReturned.printNodes();
		return toBeReturned;
	}
	
	vector<element> getElements()
	{
		return elements;
	}
	
	vector<int> getNumbers()
	{
		vector<int> toBeReturned;
		for(unsigned int i=0; i<elements.size(); i++) // goes through all elements
		{
			toBeReturned.push_back(elements.at(i).eNum);
		}
		return toBeReturned;
	}
	
	void replaceNodes(vector<int>  toBeReplaced,int toReplace)
	{
		for(int i=0;i<(int) elements.size();i++) // go through all elements
		{
			vector<int> origNodes;
			vector<int> newNodes;
			origNodes=elements.at(i).getNodes();
			newNodes.resize(origNodes.size());
			for(int j=0; j<(int) origNodes.size();j++) // for all nodes in the current element
			{
				bool replace=false;
				for(int k=0; k<(int) toBeReplaced.size(); k++) //check if the current node should be replaced
				{
					if(toBeReplaced.at(k)==origNodes.at(j)) // if one of the origNodes is the same as one of those who should be replaced
					{
						replace=true;
					}
				}
				if(replace) {newNodes.at(j)=toReplace;}
				else {newNodes.at(j)=origNodes.at(j);}
			}
			elements.at(i).setNodes(newNodes);
		}
	}
	
	int getHighestElementNumber()
	{
		int highestElementNumber=map.size()-1;
//		for(unsigned int i=0;i<elements.size();i++) //find the largest node number
//		{
//			if(elements.at(i).eNum>highestElementNumber)
//			{
//				highestElementNumber=elements.at(i).eNum;
//			}
//		}
		return highestElementNumber;
	}
	
	void deleteElement(int toDelete)
	{
		int pos=map.at(toDelete);
		int oldSize=elements.size();
		for(int i=pos;i<oldSize-1;i++) // move all nodes after the removed one down one step
		{
			elements.at(i)=elements.at(i+1);
		}
		elements.pop_back(); // deletes the last node
		reMap();
		//checkMap();
	}
	
	void reMap() // recreates the element map
	{
		//printf("Remapping\n");
		int highestElementNumber=-1;
		for(unsigned int i=0;i<elements.size();i++) //find the largest node number
		{
			if(elements.at(i).eNum>highestElementNumber)
			{
				highestElementNumber=elements.at(i).eNum;
			}
		}
		map.resize(highestElementNumber+1,-1);
		for(unsigned int i=0;i<elements.size();i++) //go through all nodes and place a location on the map
		{
			int eNum=elements.at(i).eNum;
			map.at(eNum)=i;
		}
		//checkMap();
	}
	
	int checkMap() // check map integrity
	{
		//1 = map is ok, 0 = map is corrupted
		for(int i=0;i<(int) elements.size();i++)
		{
			int eNum=elements.at(i).eNum;
			int pos=map.at(eNum);
			if(pos!=i)
			{
				printf("Element map is corrupted. (in method checkMap)\n");
				return 0;
			}	
		}
		return 1;
	}
	
	vector<element> getElementsWithNode(int node)
	{
		vector<element> elementList;
		for(int i=0;i<(int) elements.size();i++)
		{
			bool partOfList=false;
			vector<int> nodelist;
			nodelist=elements.at(i).getNodes();
			for(int j=0;j<(int)nodelist.size();j++)
			{
				if(node==nodelist.at(j))
				{
					partOfList=true;
					break;
				}
			}
			if(partOfList)
			{
				elementList.push_back(elements.at(i));
			}
		}
		return elementList;
	}
	
	vector<int> getElementsWithNodes(vector<int> nodeset,int type) //return all elements who has a node in the nodeset
	{
		//type=0 only one node of the element has to be within the nodeset
		//type=1 all nodes has to
		vector<int> elementList;
		if(type==0)
		{
			for(int i=0;i<(int) elements.size();i++)
			{
				if(intersect(elements.at(i).getNodes(),nodeset))
				{
					elementList.push_back(elements.at(i).eNum);
				}
			}
		}
		if(type==1)
		{
			for(int i=0;i<(int) elements.size();i++)
			{
				//printf("node: %d",elements.at(i).getNodes().at(0));
//				printf(",%d",elements.at(i).getNodes().at(1));
//				printf(",%d",elements.at(i).getNodes().at(2));
//				printf(",%d",elements.at(i).getNodes().at(3));
//				printf(",%d",elements.at(i).getNodes().at(4));
//				printf(",%d",elements.at(i).getNodes().at(5));
//				printf(",%d",elements.at(i).getNodes().at(6));
//				printf(",%d\n",elements.at(i).getNodes().at(7));
				if(partOf(elements.at(i).getNodes(),nodeset))
				{
					elementList.push_back(elements.at(i).eNum);
				}
			}
		}
		if(type==4)
		{
			for(int i=0;i<(int) elements.size();i++)
			{
				if(partOfSpecial(elements.at(i).getNodes(),nodeset,4))
				{
					elementList.push_back(elements.at(i).eNum);
				}
			}
		}
		return elementList;
	}
	
	vector<int> getElementsAt(double xl,double xh,double yl,double yh,double zl,double zh)
	{
		vector<int> elementList;
//		vector<int> leftNodes;
//		vector<int> rightNodes;
//		
//		leftNodes=nodes.getNodesToTheLeftOf(xh);//find nodes to the left of coord
//		rightNodes=nodes.getNodesToTheRightOf(xl);//find nodes to the right of coord
//		topNodes=nodes.getNodesAbove(zl);
//		botNodes=nodes.getNodesBelow(zh);
//		frontNodes=nodes.getNodesInfrontOf(yl);
//		rearNodes=nodes.getNodesBehibdOf(yh);
//		
//		
//
//		//find elements with nodes in both groups
//		for(int i=0;i<(int) elements.size();i++)
//		{
//			if(intersect(leftNodes,elements.at(i).getNodes()) && intersect(rightNodes,elements.at(i).getNodes()))
//			{
//				elementList.push_back(elements.at(i).eNum);
//			}
//		}
		for(int i=0;i<(int) elements.size();i++)
		{
			coord cg;
			cg=elements.at(i).getCenterOfGravity();
			double tol=elements.at(i).getSize()*0.49;
			double uptol=elements.at(i).getSize()*0.50;
			//printf("cg=%f %f %f\n",cg.x,cg.y,cg.z);
			if((xl<cg.x+uptol) && (xh>cg.x-tol) && (yl<cg.y+uptol) && (yh>cg.y-uptol) && (zl<cg.z+uptol) && (zh>cg.z-tol))
				elementList.push_back(elements.at(i).eNum);		
		}

		
		return elementList;
	}
	
	vector<int> getElementsToTheRightOf(double coord, nodeBox nodes)
	{
		vector<int> elementList;
		vector<int> rightNodes;
		
		rightNodes=nodes.getNodesToTheRightOf(coord);//find nodes to the right of coord
		
		for(int i=0;i<(int) elements.size();i++)
		{
			if(partOf(elements.at(i).getNodes(),rightNodes))
			{
				elementList.push_back(elements.at(i).eNum);
			}
		}
		return elementList;
	}
	
	vector<int> eselY(double coord, nodeBox nodes)
	{
		vector<int> elementList;
		vector<int> upNodes;
		vector<int> downNodes;
		
		upNodes=nodes.nselY(coord,100000,1e-6);
		downNodes=nodes.nselY(-100000,coord,1e-6);

		//printf("upNodes.size()=%d downNodes.size()=%d\n",upNodes.size(),downNodes.size());
		
		//find elements with nodes in both groups
		for(int i=0;i<(int) elements.size();i++)
		{
			if(intersect(upNodes,elements.at(i).getNodes()) && intersect(downNodes,elements.at(i).getNodes())) //this is the part thats wrong
			{
				elementList.push_back(elements.at(i).eNum);
			}
		}
		return elementList;
	}
	
	void updateCenterOfGravity(nodeBox nBox)
	{
		//printf("Inside update bla bla\n");
		for(unsigned int i=0;i<elements.size();i++)
		{
			//printf("updating element %d\n",elements.at(i).eNum);
			elements.at(i).calcCenterOfGravity(nBox);
		}
	}
	
	void shiftElementNumbers(int shift)
	{
		for(unsigned int i=0; i<elements.size();i++)
		{
			elements.at(i).eNum=elements.at(i).eNum+shift;
		}
	}
	
	void shiftNodeNumbers(int shift)
	{
		for(unsigned int i=0;i<elements.size();i++)
		{
			elements.at(i).shiftNodeNumbers(shift);
		}
	}
	
	void addElements(vector<element> toAdd)
	{
		for(unsigned int i=0; i<toAdd.size();i++)
		{
			addElement(toAdd.at(i));
		}
	}

	void mirror() //change the node numbering of all elements in box
	{
		for(unsigned int i=0; i<elements.size();i++)
		{
			elements.at(i).mirror();
		}
	}
		
};

struct corner
{
	double x,z;
	
	corner()
	{
		x=0;
		z=0;
	}
};

struct mesh
{
	elementBox elements;
	nodeBox nodes;
	double thickness;
	double cohesiveThickness;
	double cohesiveFraction;
	double substrateThickness;
	double length;
	double width;
	double tol;
	int ldiv,wdiv,tdiv,reps;
	bool dcb,toMirror;
	bool flatCohesiveLayer;
	
	int divisions;
	vector<refPoint> refPoints;
	vector<int> nodeNumbers;
	vector<int> bottomNodes;
	vector<int> cohesiveNodes;
	vector<int> inletLower;
	vector<int> inletUpper;
	vector<int> blockElements;
	vector<int> blockNodes; 
	vector<int> cohesiveElements;

	mesh()
	{
		thickness=-1;
		length=-1;
		width=-1;
		tol=1e-6;
		tdiv=-1;
		wdiv=-1;
		ldiv=-1;
		reps=1; //reps=1 will not cause any replication
		cohesiveThickness=-1;
		cohesiveFraction=1; //assume no crack
		dcb=false;
		toMirror=false; //if the mesh should be mirrored
		flatCohesiveLayer=false;
	}

	int generateBaseMesh() //generates a base hex mesh to be refined at a later stage
	{
		tdiv=divisions;
		double eLength=thickness/divisions;
		printf("tdiv: %d\n",tdiv);
		ldiv=int(length/eLength+0.5);
		printf("ldiv: %d\n",ldiv);
		wdiv=int(width/eLength+0.5);
		printf("wdiv: %d\n",wdiv);
		printf("Generating base mesh...\n");
		int current_nNum=1;
		node currentNode;

		if(length==0.0) //if the mesh is 2D
		{
			ldiv=0;
		}

		for(int k=0;k<tdiv+1;k++)
		{
			for(int j=0;j<ldiv+1;j++)
			{
				for(int i=0;i<wdiv+1;i++)
				{
					currentNode.nNum=current_nNum;
					currentNode.x= width*(i/((double) wdiv)-1);
					if(ldiv>0){currentNode.y= length*j/((double) ldiv);}
					else{currentNode.y=0.0;}
					currentNode.z= thickness*(k/((double) tdiv)-1);
					nodes.addNode(currentNode);
					current_nNum++;
				}
			}
			//printf("Ply %d done.\n",k);
		}
		
		int currentElementNum=1;
		if(ldiv>0)
		{
			for(int k=1;k<tdiv+1;k++)
			{
				for(int j=1;j<ldiv+1;j++)
				{
					for(int i=1;i<wdiv+1;i++)
					{	
						element currentElement;
						currentElement.setNumber(currentElementNum);
						
						currentElement.nodes.push_back(i+(j-1)*(wdiv+1)+(k-1)*(ldiv+1)*(wdiv+1));
						currentElement.nodes.push_back((i+1)+(j-1)*(wdiv+1)+(k-1)*(ldiv+1)*(wdiv+1));
						currentElement.nodes.push_back((i+1)+(j-1+1)*(wdiv+1)+(k-1)*(ldiv+1)*(wdiv+1));
						currentElement.nodes.push_back(i+(j-1+1)*(wdiv+1)+(k-1)*(ldiv+1)*(wdiv+1));
												
						currentElement.nodes.push_back(i+(j-1)*(wdiv+1)+(k)*(ldiv+1)*(wdiv+1));
						currentElement.nodes.push_back((i+1)+(j-1)*(wdiv+1)+(k)*(ldiv+1)*(wdiv+1));
						currentElement.nodes.push_back((i+1)+(j-1+1)*(wdiv+1)+(k)*(ldiv+1)*(wdiv+1));
						currentElement.nodes.push_back(i+(j-1+1)*(wdiv+1)+(k)*(ldiv+1)*(wdiv+1));
						
						currentElement.nodesPerElement=8;

						
						elements.addElement(currentElement);
						currentElementNum++;
					}
				}
			}
		}
		else
		{
			for(int k=1;k<tdiv+1;k++)
			{
				int j=1;
				for(int i=1;i<wdiv+1;i++)
				{	
					element currentElement;
					currentElement.setNumber(currentElementNum);
					
					currentElement.nodes.push_back(i+(j-1)*(wdiv+1)+(k-1)*(ldiv+1)*(wdiv+1));
					currentElement.nodes.push_back((i+1)+(j-1)*(wdiv+1)+(k-1)*(ldiv+1)*(wdiv+1));
					currentElement.nodes.push_back((i+1)+(j-1+1)*(wdiv+1)+(k-1)*(ldiv+1)*(wdiv+1));
					currentElement.nodes.push_back(i+(j-1+1)*(wdiv+1)+(k-1)*(ldiv+1)*(wdiv+1));
											
					currentElement.nodesPerElement=4;
					elements.addElement(currentElement);
					currentElementNum++;
				}
			
			}
			
		}
		
		printf("Base mesh nodes generated. %d nodes and %d elements.\n",current_nNum-1,currentElementNum-1);
		return 0;
	}
	
	void refineMesh()
	{
		printf("Refinig mesh...");
		for(int i=0;i<(int)refPoints.size();i++)
		{
			refPoint ref;
			ref=refPoints.at(i);
			int refType=refPoints.at(i).type;
			vector<int> toBeTransmogedX;
			vector<int> toBeTransmogedZ;
			vector<int> toBeSplitted;
			
			
			printf("Updating center of gravity..\n");
			elements.updateCenterOfGravity(nodes);
			printf("done.\n");
			
			
			
			
			//transmogrifying
			printf("ref.x, %f\n",ref.x);
			toBeTransmogedX=getElementsAt(ref.x-tol,ref.x+tol,-1.0,1000000.0,ref.z,1000000.0);
			toBeTransmogedZ=getElementsAt(ref.x,100000000,-1.0,1000000.0,ref.z-tol,ref.z+tol);

			//printf("toBeTransmogedX %d",toBeTransmogedX.size());
			
			vector<int> cornerElementNumbers; //We dont want to transmog the corner elements
			cornerElementNumbers=getIntersection(toBeTransmogedX,toBeTransmogedZ);

			toBeTransmogedX=subtract(toBeTransmogedX,cornerElementNumbers);
			toBeTransmogedZ=subtract(toBeTransmogedZ,cornerElementNumbers);
			
			
			int refTypeZ=-1;
			if(refType==0)
			{
				refTypeZ=3;
			}
			else if(refType==1)
			{
				refTypeZ=2;
			}
			else
			{
				printf("Whoa! this refType does not exist.\n");	
			}
			
			//to split
			toBeSplitted=getElementsAt(ref.x,10000000,-1.0,1000000.0,ref.z,10000000);
			toBeSplitted=subtract(toBeSplitted,cornerElementNumbers);
			toBeSplitted=subtract(toBeSplitted,toBeTransmogedX);
			toBeSplitted=subtract(toBeSplitted,toBeTransmogedZ);
			
			transmogElements(toBeTransmogedX,refType);
			transmogElements(toBeTransmogedZ,refTypeZ);
			if(refType==0) //the corner elements only needs to be transm. in this case
				transmogElements(cornerElementNumbers,4);
			
			splitElements(toBeSplitted,refType);
			mergeNodes();
			
		}

		printf(" done.\n");
	}
	
	void setWidth(double w)
	{
		width=w;
	}
	
	void setThickness(double t)
	{
		thickness=t;
	}
	
	void setLength(double l)
	{
		length=l;
	}
	
	void setDivisions(int d)
	{
		divisions=d;
	}
	
	void setTolerance(double tolerance)
	{
		tol=tolerance;
	}
	
	void setRefinement(vector<refPoint> refs)
	{
		refPoints=refs;
	}
	
	void setReps(int r)
	{
		reps=r;
	}
	
	void setCohesiveThickness(double cthick)
	{
		if(cthick==0) //if the cohesive layer is flat
		{	
			printf("Flat cohesive layer\n");
			flatCohesiveLayer=true; 
			cohesiveThickness=1e-4;
		}
		else
			cohesiveThickness=cthick;
	}
	void setCohesiveFraction(double cf)
	{
		cohesiveFraction=cf;
	}
	void setSubstrateThickness(double sthick)
	{
		substrateThickness=sthick;
	}
	
	void toggleDCB()
	{
		dcb=true;
	}

	void toggleMirror()
	{
		toMirror=true;
	}
	
	void splitElement(int eToSplit, int type) // Splitts an element into more elements
	{
		//printf("split element %d, type %d, ldiv %d",eToSplit,type,ldiv);
		if(ldiv>0) //3D elements
		{
			int tdiv=1;
			int ldiv=1;
			int wdiv=1;
			if(type==0)
			{
				ldiv=3;
			}
			else if(type==1) //if it is split the other way (thickness wise)
			{
				tdiv=3;
				ldiv=1;
				wdiv=3;
			}
			else if(type==2) //split in every way
			{
				tdiv=3;
				ldiv=3;
				wdiv=3;
			}
			else
				printf("Error in split element. Type of split unknown. (type=%d)",type);
			
			
			//printf("Splitting element %d into %d elements",eToSplit,tdiv*ldiv*wdiv);
			
			node firstNode;
			element elementToSplit;
			elementToSplit=elements.getElement(eToSplit);
			int firstNodeNum=elementToSplit.getNodes().at(0); // the first node of the element that is about to be splitted
			firstNode=nodes.getNode(firstNodeNum);
			
			double width=nodes.getNode(elementToSplit.getNodes().at(1)).x-nodes.getNode(elementToSplit.getNodes().at(0)).x;
			double length=nodes.getNode(elementToSplit.getNodes().at(3)).y-nodes.getNode(elementToSplit.getNodes().at(0)).y;
			double thickness=nodes.getNode(elementToSplit.getNodes().at(5)).z-nodes.getNode(elementToSplit.getNodes().at(0)).z;
			
			int current_nNum=nodes.getHighestNodeNumber()+1;
			int highestNodeNumber=current_nNum;
			node currentNode;
			for(int k=0;k<tdiv+1;k++)
			{
				for(int j=0;j<ldiv+1;j++)
				{
					for(int i=0;i<wdiv+1;i++)
					{
						currentNode.nNum=current_nNum;
						currentNode.x= width*i/((double) wdiv)+firstNode.x;
						currentNode.y= length*j/((double) ldiv)+firstNode.y;
						currentNode.z= thickness*k/((double) tdiv)+firstNode.z;
						nodes.addNode(currentNode);
						current_nNum++;
					}
				}
			}
			
			int currentElementNum=elements.getHighestElementNumber()+1;
			for(int k=1;k<tdiv+1;k++)
			{
				for(int j=1;j<ldiv+1;j++)
				{
					for(int i=highestNodeNumber;i<wdiv+highestNodeNumber;i++)
					{	
						element currentElement;
						currentElement.setNumber(currentElementNum);
						
						currentElement.nodes.push_back(i+(j-1)*(wdiv+1)+(k-1)*(ldiv+1)*(wdiv+1));
						currentElement.nodes.push_back((i+1)+(j-1)*(wdiv+1)+(k-1)*(ldiv+1)*(wdiv+1));
						currentElement.nodes.push_back((i+1)+(j-1+1)*(wdiv+1)+(k-1)*(ldiv+1)*(wdiv+1));
						currentElement.nodes.push_back(i+(j-1+1)*(wdiv+1)+(k-1)*(ldiv+1)*(wdiv+1));
						
						currentElement.nodes.push_back(i+(j-1)*(wdiv+1)+(k)*(ldiv+1)*(wdiv+1));
						currentElement.nodes.push_back((i+1)+(j-1)*(wdiv+1)+(k)*(ldiv+1)*(wdiv+1));
						currentElement.nodes.push_back((i+1)+(j-1+1)*(wdiv+1)+(k)*(ldiv+1)*(wdiv+1));
						currentElement.nodes.push_back(i+(j-1+1)*(wdiv+1)+(k)*(ldiv+1)*(wdiv+1));

						
						elements.addElement(currentElement);
						currentElementNum++;
						
						//currentElement.printNodes();
					}
				}
			}
		}
		else //2D elements
		{
			int tdiv=1;
			int ldiv=1;
			int wdiv=1;
			if(type==0)
			{
				ldiv=3;
			}
			else if(type==1) //if it is split the other way (thickness wise)
			{
				tdiv=3;
				ldiv=1;
				wdiv=3;
			}
			else if(type==2) //split in every way
			{
				tdiv=3;
				ldiv=3;
				wdiv=3;
			}
			else
				printf("Error in split element. Type of split unknown. (type=%d)",type);
			
			
			//printf("Splitting element %d into %d elements",eToSplit,tdiv*ldiv*wdiv);
			
			node firstNode;
			element elementToSplit;
			elementToSplit=elements.getElement(eToSplit);
			int firstNodeNum=elementToSplit.getNodes().at(0); // the first node of the element that is about to be splitted
			firstNode=nodes.getNode(firstNodeNum);
			double width=nodes.getNode(elementToSplit.getNodes().at(1)).x-nodes.getNode(elementToSplit.getNodes().at(0)).x;
			//double length=nodes.getNode(elementToSplit.getNodes().at(3)).y-nodes.getNode(elementToSplit.getNodes().at(0)).y;
			double thickness=nodes.getNode(elementToSplit.getNodes().at(3)).z-nodes.getNode(elementToSplit.getNodes().at(0)).z;

			
			int current_nNum=nodes.getHighestNodeNumber()+1;
			int highestNodeNumber=current_nNum;
			node currentNode;
			for(int k=0;k<tdiv+1;k++)
			{
				for(int j=0;j<ldiv+1;j++)
				{
					for(int i=0;i<wdiv+1;i++)
					{
						//printf("%d,%d,%d",k,j,i)
						currentNode.nNum=current_nNum;
						currentNode.x= width*i/((double) wdiv)+firstNode.x;
						//currentNode.y= length*j/((double) ldiv)+firstNode.y;
						currentNode.y=firstNode.y;
						currentNode.z= thickness*k/((double) tdiv)+firstNode.z;
						nodes.addNode(currentNode);
						current_nNum++;
					}
				}
			}
			
			int currentElementNum=elements.getHighestElementNumber()+1;
			for(int k=1;k<tdiv+1;k++)
			{
				for(int j=1;j<ldiv+1;j++)
				{
					for(int i=highestNodeNumber;i<wdiv+highestNodeNumber;i++)
					{	
						element currentElement;
						currentElement.setNumber(currentElementNum);
						
						currentElement.nodes.push_back(i+(j-1)*(wdiv+1)+(k-1)*(ldiv+1)*(wdiv+1));
						currentElement.nodes.push_back((i+1)+(j-1)*(wdiv+1)+(k-1)*(ldiv+1)*(wdiv+1));
						//currentElement.nodes.push_back((i+1)+(j-1+1)*(wdiv+1)+(k-1)*(ldiv+1)*(wdiv+1));
						//currentElement.nodes.push_back(i+(j-1+1)*(wdiv+1)+(k-1)*(ldiv+1)*(wdiv+1));
						
						
						currentElement.nodes.push_back((i+1)+(j-1)*(wdiv+1)+(k)*(ldiv+1)*(wdiv+1));
						currentElement.nodes.push_back(i+(j-1)*(wdiv+1)+(k)*(ldiv+1)*(wdiv+1));
						//currentElement.nodes.push_back((i+1)+(j-1+1)*(wdiv+1)+(k)*(ldiv+1)*(wdiv+1));
						//currentElement.nodes.push_back(i+(j-1+1)*(wdiv+1)+(k)*(ldiv+1)*(wdiv+1));

						
						elements.addElement(currentElement);
						currentElementNum++;
						
						//currentElement.printNodes();
					}
				}
			}
		}
		elements.deleteElement(eToSplit);
		//printf(" done.\n");
	}
	
	
	void transmogElement(int eToRefine, int type)
	{
		//printf("Trying to transmogrify element %d\n",eToRefine);
		if(ldiv>0) //then this is not a 2D mesh
		{
			if(type==0)
			{
				element oldElement;
				oldElement=elements.getElement(eToRefine);
				//printf("In method refine element:\n");
				//oldElement.printNodes();
						
				vector<node> oldNodes; // nodes in the element to be refined
				vector<int> oldNodeNumbers; // list of node numbers in the element to be refined
				
				oldNodeNumbers=oldElement.getNodes();
		
				for(int i=0;i<8;i++)
				{	
					node tempNode;			
					int tempNumber=oldNodeNumbers.at(i);
					tempNode=nodes.getNode(tempNumber);
					oldNodes.push_back(tempNode);
				}
				
				//create new nodes
				node node1;
				node1.nNum=nodes.size();
				node1.x=(oldNodes.at(1).x-oldNodes.at(0).x)/2+oldNodes.at(0).x;
				node1.y=(oldNodes.at(3).y-oldNodes.at(0).y)/3+oldNodes.at(0).y;
				node1.z=oldNodes.at(0).z;
				nodes.addNode(node1);
				
				node node2;
				node2.nNum=nodes.size();
				node2.x=oldNodes.at(1).x;
				node2.y=(oldNodes.at(3).y-oldNodes.at(0).y)/3+oldNodes.at(0).y;
				node2.z=oldNodes.at(0).z;
				nodes.addNode(node2);
				
				node node3;
				node3.nNum=nodes.size();
				node3.x=oldNodes.at(1).x;
				node3.y=(oldNodes.at(3).y-oldNodes.at(0).y)/3*2+oldNodes.at(0).y;
				node3.z=oldNodes.at(0).z;
				nodes.addNode(node3);
		
				node node4;
				node4.nNum=nodes.size();
				node4.x=(oldNodes.at(1).x-oldNodes.at(0).x)/2+oldNodes.at(0).x;
				node4.y=(oldNodes.at(3).y-oldNodes.at(0).y)/3*2+oldNodes.at(0).y;
				node4.z=oldNodes.at(0).z;
				nodes.addNode(node4);		
				
				node node5;
				node5.nNum=nodes.size();
				node5.x=(oldNodes.at(1).x-oldNodes.at(0).x)/2+oldNodes.at(0).x;
				node5.y=(oldNodes.at(3).y-oldNodes.at(0).y)/3+oldNodes.at(0).y;
				node5.z=oldNodes.at(4).z;
				nodes.addNode(node5);
				
				node node6;
				node6.nNum=nodes.size();
				node6.x=oldNodes.at(1).x;
				node6.y=(oldNodes.at(3).y-oldNodes.at(0).y)/3+oldNodes.at(0).y;
				node6.z=oldNodes.at(4).z;
				nodes.addNode(node6);
				
				node node7;
				node7.nNum=nodes.size();
				node7.x=oldNodes.at(1).x;
				node7.y=(oldNodes.at(3).y-oldNodes.at(0).y)/3*2+oldNodes.at(0).y;
				node7.z=oldNodes.at(4).z;
				nodes.addNode(node7);
		
				node node8;
				node8.nNum=nodes.size();
				node8.x=(oldNodes.at(1).x-oldNodes.at(0).x)/2+oldNodes.at(0).x;
				node8.y=(oldNodes.at(3).y-oldNodes.at(0).y)/3*2+oldNodes.at(0).y;
				node8.z=oldNodes.at(4).z;
				nodes.addNode(node8);	
				
				//make new elements
				element bot;
				bot.setNumber(eToRefine);
				vector<int> botNodes;
				botNodes=oldNodeNumbers;
				botNodes.at(1)=node1.nNum;
				botNodes.at(2)=node4.nNum;
				botNodes.at(5)=node5.nNum;
				botNodes.at(6)=node8.nNum;
				bot.setNodes(botNodes);
				elements.replaceElement(bot);
				
				int currentNumberofElements=elements.getHighestElementNumber();
				element rightFlank;
				rightFlank.setNumber(currentNumberofElements+1);
				vector<int> rightFlankNodes;
				rightFlankNodes.resize(8,-1);
				rightFlankNodes.at(0)=oldNodeNumbers.at(0);
				rightFlankNodes.at(1)=oldNodeNumbers.at(1);
				rightFlankNodes.at(2)=node2.nNum;
				rightFlankNodes.at(3)=node1.nNum;
				rightFlankNodes.at(4)=oldNodeNumbers.at(4);
				rightFlankNodes.at(5)=oldNodeNumbers.at(5);
				rightFlankNodes.at(6)=node6.nNum;
				rightFlankNodes.at(7)=node5.nNum;
				rightFlank.setNodes(rightFlankNodes);
				elements.addElement(rightFlank);
				
				element leftFlank;
				leftFlank.setNumber(currentNumberofElements+2);
				vector<int> leftFlankNodes;
				leftFlankNodes.resize(8,-1);
				leftFlankNodes.at(0)=node4.nNum;
				leftFlankNodes.at(1)=node3.nNum;
				leftFlankNodes.at(2)=oldNodeNumbers.at(2);
				leftFlankNodes.at(3)=oldNodeNumbers.at(3);
				leftFlankNodes.at(4)=node8.nNum;
				leftFlankNodes.at(5)=node7.nNum;
				leftFlankNodes.at(6)=oldNodeNumbers.at(6);
				leftFlankNodes.at(7)=oldNodeNumbers.at(7);
				leftFlank.setNodes(leftFlankNodes);
				elements.addElement(leftFlank);
				
				element middle;
				middle.setNumber(currentNumberofElements+3);
				vector<int> middleNodes;
				middleNodes.resize(8,-1);
				middleNodes.at(0)=node1.nNum;
				middleNodes.at(1)=node2.nNum;
				middleNodes.at(2)=node3.nNum;
				middleNodes.at(3)=node4.nNum;
				middleNodes.at(4)=node5.nNum;
				middleNodes.at(5)=node6.nNum;
				middleNodes.at(6)=node7.nNum;
				middleNodes.at(7)=node8.nNum;
				middle.setNodes(middleNodes);
				elements.addElement(middle);
			}
			else if(type==1)
			{
				//printf("Trying to transmogrify element %d\n",eToRefine);
				element oldElement;
				oldElement=elements.getElement(eToRefine);
				//printf("In method refine element:\n");
				//oldElement.printNodes();
						
				vector<node> oldNodes; // nodes in the element to be refined
				vector<int> oldNodeNumbers; // list of node numbers in the element to be refined
				
				oldNodeNumbers=oldElement.getNodes();

				
				for(int i=0;i<oldElement.nodesPerElement;i++)
				{	
					node tempNode;			
					int tempNumber=oldNodeNumbers.at(i);
					tempNode=nodes.getNode(tempNumber);
					oldNodes.push_back(tempNode);
				}
				
				//create new nodes
				node node1;
				double h=oldNodes.at(4).z-oldNodes.at(0).z;
				double w=oldNodes.at(1).x-oldNodes.at(0).x;
				double offset=0;
				node1.nNum=nodes.size();
				node1.x=w/2+oldNodes.at(0).x+offset;
				node1.z=h/3+oldNodes.at(0).z;
				node1.y=oldNodes.at(0).y;
				nodes.addNode(node1);
				
				node node2;
				node2.nNum=nodes.size();
				node2.x=w+oldNodes.at(0).x+offset;
				node2.z=h/3+oldNodes.at(0).z;
				node2.y=oldNodes.at(0).y;
				nodes.addNode(node2);
				
				node node3;
				node3.nNum=nodes.size();
				node3.x=w+oldNodes.at(0).x+offset;
				node3.z=h/3+oldNodes.at(0).z;
				node3.y=oldNodes.at(3).y;
				nodes.addNode(node3);
		
				node node4;
				node4.nNum=nodes.size();
				node4.x=w/2+oldNodes.at(0).x+offset;
				node4.z=h/3+oldNodes.at(0).z;
				node4.y=oldNodes.at(3).y;
				nodes.addNode(node4);		
				
				
				node node5;
				node5.nNum=nodes.size();
				node5.x=w/2+oldNodes.at(0).x+offset;
				node5.z=h/3.0*2.0+oldNodes.at(0).z+offset;
				node5.y=oldNodes.at(0).y;
				nodes.addNode(node5);
				
				node node6;
				node6.nNum=nodes.size();
				node6.x=w+oldNodes.at(0).x+offset;
				node6.z=h/3.0*2.0+oldNodes.at(0).z+offset;
				node6.y=oldNodes.at(0).y;
				nodes.addNode(node6);
				
				node node7;
				node7.nNum=nodes.size();
				node7.x=w+oldNodes.at(0).x+offset;
				node7.z=h/3.0*2.0+oldNodes.at(0).z+offset;
				node7.y=oldNodes.at(3).y;
				nodes.addNode(node7);
		
				node node8;
				node8.nNum=nodes.size();
				node8.x=w/2+oldNodes.at(0).x+offset;
				node8.z=h/3.0*2.0+oldNodes.at(0).z+offset;
				node8.y=oldNodes.at(3).y;
				nodes.addNode(node8);	
			
				
				//make new elements
				element bot;
				bot.setNumber(eToRefine);
				vector<int> botNodes;
				botNodes=oldNodeNumbers;
				botNodes.at(1)=node1.nNum;
				botNodes.at(2)=node4.nNum;
				botNodes.at(5)=node5.nNum;
				botNodes.at(6)=node8.nNum;
				bot.setNodes(botNodes);
				elements.replaceElement(bot);
				
				int currentNumberofElements=elements.getHighestElementNumber();
				element rightFlank;
				rightFlank.setNumber(currentNumberofElements+1);
				vector<int> rightFlankNodes;
				rightFlankNodes.resize(8,-1);
				rightFlankNodes.at(0)=oldNodeNumbers.at(0);
				rightFlankNodes.at(1)=oldNodeNumbers.at(1);
				rightFlankNodes.at(2)=oldNodeNumbers.at(2);
				rightFlankNodes.at(3)=oldNodeNumbers.at(3);
				rightFlankNodes.at(4)=node1.nNum;
				rightFlankNodes.at(5)=node2.nNum;
				rightFlankNodes.at(6)=node3.nNum;
				rightFlankNodes.at(7)=node4.nNum;
				rightFlank.setNodes(rightFlankNodes);
				elements.addElement(rightFlank);
				
				element leftFlank;
				leftFlank.setNumber(currentNumberofElements+2);
				vector<int> leftFlankNodes;
				leftFlankNodes.resize(8,-1);
				leftFlankNodes.at(0)=node5.nNum;
				leftFlankNodes.at(1)=node6.nNum;
				leftFlankNodes.at(2)=node7.nNum;
				leftFlankNodes.at(3)=node8.nNum;
				leftFlankNodes.at(4)=oldNodeNumbers.at(4);
				leftFlankNodes.at(5)=oldNodeNumbers.at(5);
				leftFlankNodes.at(6)=oldNodeNumbers.at(6);
				leftFlankNodes.at(7)=oldNodeNumbers.at(7);
				leftFlank.setNodes(leftFlankNodes);
				elements.addElement(leftFlank);
				
				element middle;
				middle.setNumber(currentNumberofElements+3);
				vector<int> middleNodes;
				middleNodes.resize(8,-1);
				middleNodes.at(0)=node1.nNum;
				middleNodes.at(1)=node2.nNum;
				middleNodes.at(2)=node3.nNum;
				middleNodes.at(3)=node4.nNum;
				middleNodes.at(4)=node5.nNum;
				middleNodes.at(5)=node6.nNum;
				middleNodes.at(6)=node7.nNum;
				middleNodes.at(7)=node8.nNum;
				middle.setNodes(middleNodes);
				elements.addElement(middle);
			}
			
			else if(type==2)
			{
				//printf("Trying to transmogrify element %d\n",eToRefine);
				element oldElement;
				oldElement=elements.getElement(eToRefine);
				//printf("In method refine element:\n");
				//oldElement.printNodes();
						
				vector<node> oldNodes; // nodes in the element to be refined
				vector<int> oldNodeNumbers; // list of node numbers in the element to be refined
				
				oldNodeNumbers=oldElement.getNodes();
		
				for(int i=0;i<8;i++)
				{	
					node tempNode;			
					int tempNumber=oldNodeNumbers.at(i);
					tempNode=nodes.getNode(tempNumber);
					oldNodes.push_back(tempNode);
				}
				
				//create new nodes
				node node1;
				double h=oldNodes.at(4).z-oldNodes.at(0).z;
				double w=oldNodes.at(1).x-oldNodes.at(0).x;

				node1.nNum=nodes.size();
				node1.x=w/3+oldNodes.at(0).x;
				node1.z=h/2+oldNodes.at(0).z;
				node1.y=oldNodes.at(0).y;
				nodes.addNode(node1);
				
				node node2;
				node2.nNum=nodes.size();
				node2.x=w/3*2+oldNodes.at(0).x;
				node2.z=h/2+oldNodes.at(0).z;
				node2.y=oldNodes.at(0).y;
				nodes.addNode(node2);
				
				node node3;
				node3.nNum=nodes.size();
				node3.x=w/3*2+oldNodes.at(0).x;
				node3.z=h/2+oldNodes.at(0).z;
				node3.y=oldNodes.at(3).y;
				nodes.addNode(node3);
		
				node node4;
				node4.nNum=nodes.size();
				node4.x=w/3+oldNodes.at(0).x;
				node4.z=h/2+oldNodes.at(0).z;
				node4.y=oldNodes.at(3).y;
				nodes.addNode(node4);		
				
				node node5;
				node5.nNum=nodes.size();
				node5.x=w/3+oldNodes.at(0).x;
				node5.z=oldNodes.at(4).z;
				node5.y=oldNodes.at(0).y;
				nodes.addNode(node5);
				
				node node6;
				node6.nNum=nodes.size();
				node6.x=w/3*2+oldNodes.at(0).x;
				node6.z=oldNodes.at(4).z;
				node6.y=oldNodes.at(0).y;
				nodes.addNode(node6);
				
				node node7;
				node7.nNum=nodes.size();
				node7.x=w/3*2+oldNodes.at(0).x;
				node7.z=oldNodes.at(7).z;
				node7.y=oldNodes.at(3).y;
				nodes.addNode(node7);
		
				node node8;
				node8.nNum=nodes.size();
				node8.x=w/3+oldNodes.at(0).x;
				node8.z=oldNodes.at(7).z;
				node8.y=oldNodes.at(3).y;
				nodes.addNode(node8);	
				
				//make new elements
				element bot;
				bot.setNumber(eToRefine);
				vector<int> botNodes;
				botNodes=oldNodeNumbers;
				botNodes.at(4)=node1.nNum;
				botNodes.at(5)=node2.nNum;
				botNodes.at(6)=node3.nNum;
				botNodes.at(7)=node4.nNum;
				bot.setNodes(botNodes);
				elements.replaceElement(bot);
				
				int currentNumberofElements=elements.getHighestElementNumber();
				element rightFlank;
				rightFlank.setNumber(currentNumberofElements+1);
				vector<int> rightFlankNodes;
				rightFlankNodes.resize(8,-1);
				rightFlankNodes.at(0)=node2.nNum;
				rightFlankNodes.at(1)=oldNodeNumbers.at(1);
				rightFlankNodes.at(2)=oldNodeNumbers.at(2);
				rightFlankNodes.at(3)=node3.nNum;
				rightFlankNodes.at(4)=node6.nNum;
				rightFlankNodes.at(5)=oldNodeNumbers.at(5);
				rightFlankNodes.at(6)=oldNodeNumbers.at(6);
				rightFlankNodes.at(7)=node7.nNum;
				rightFlank.setNodes(rightFlankNodes);
				elements.addElement(rightFlank);
				
				element leftFlank;
				leftFlank.setNumber(currentNumberofElements+2);
				vector<int> leftFlankNodes;
				leftFlankNodes.resize(8,-1);
				leftFlankNodes.at(0)=oldNodeNumbers.at(0);
				leftFlankNodes.at(1)=node1.nNum;
				leftFlankNodes.at(2)=node4.nNum;
				leftFlankNodes.at(3)=oldNodeNumbers.at(3);
				leftFlankNodes.at(4)=oldNodeNumbers.at(4);
				leftFlankNodes.at(5)=node5.nNum;
				leftFlankNodes.at(6)=node8.nNum;
				leftFlankNodes.at(7)=oldNodeNumbers.at(7);
				leftFlank.setNodes(leftFlankNodes);
				elements.addElement(leftFlank);
				
				element middle;
				middle.setNumber(currentNumberofElements+3);
				vector<int> middleNodes;
				middleNodes.resize(8,-1);
				middleNodes.at(0)=node1.nNum;
				middleNodes.at(1)=node2.nNum;
				middleNodes.at(2)=node3.nNum;
				middleNodes.at(3)=node4.nNum;
				middleNodes.at(4)=node5.nNum;
				middleNodes.at(5)=node6.nNum;
				middleNodes.at(6)=node7.nNum;
				middleNodes.at(7)=node8.nNum;
				middle.setNodes(middleNodes);
				elements.addElement(middle);
			}
			else if(type==3)
			{
				//printf("Trying to transmogrify element %d\n",eToRefine);
				element oldElement;
				oldElement=elements.getElement(eToRefine);
				//printf("In method refine element:\n");
				//oldElement.printNodes();
						
				vector<node> oldNodes; // nodes in the element to be refined
				vector<int> oldNodeNumbers; // list of node numbers in the element to be refined
				
				oldNodeNumbers=oldElement.getNodes();
		
				for(int i=0;i<8;i++)
				{	
					node tempNode;			
					int tempNumber=oldNodeNumbers.at(i);
					tempNode=nodes.getNode(tempNumber);
					oldNodes.push_back(tempNode);
				}
				
				//create new nodes
				node node1;
				double h=oldNodes.at(4).z-oldNodes.at(0).z;
				double w=oldNodes.at(1).x-oldNodes.at(0).x;
				double l=oldNodes.at(3).y-oldNodes.at(0).y;

				node1.nNum=nodes.size();
				node1.x=oldNodes.at(0).x;
				node1.y=l/3+oldNodes.at(0).y;
				node1.z=h/2+oldNodes.at(0).z;
				
				nodes.addNode(node1);
				
				node node2;
				node2.nNum=nodes.size();
				node2.x=w+oldNodes.at(0).x;
				node2.y=l/3+oldNodes.at(0).y;
				node2.z=h/2+oldNodes.at(0).z;
				nodes.addNode(node2);
				
				node node3;
				node3.nNum=nodes.size();
				node3.x=w+oldNodes.at(0).x;
				node3.y=l/3*2+oldNodes.at(0).y;
				node3.z=h/2+oldNodes.at(0).z;
				nodes.addNode(node3);
		
				node node4;
				node4.nNum=nodes.size();
				node4.x=oldNodes.at(0).x;
				node4.y=l/3*2+oldNodes.at(0).y;
				node4.z=h/2+oldNodes.at(0).z;
				nodes.addNode(node4);		
				
				node node5;
				node5.nNum=nodes.size();
				node5.x=oldNodes.at(0).x;
				node5.y=l/3+oldNodes.at(0).y;
				node5.z=h+oldNodes.at(0).z;
				nodes.addNode(node5);
				
				node node6;
				node6.nNum=nodes.size();
				node6.x=w+oldNodes.at(0).x;
				node6.y=l/3+oldNodes.at(0).y;
				node6.z=h+oldNodes.at(0).z;
				nodes.addNode(node6);
				
				node node7;
				node7.nNum=nodes.size();
				node7.x=w+oldNodes.at(0).x;
				node7.y=l/3*2+oldNodes.at(0).y;
				node7.z=h+oldNodes.at(0).z;
				nodes.addNode(node7);
		
				node node8;
				node8.nNum=nodes.size();
				node8.x=oldNodes.at(0).x;
				node8.y=l/3*2+oldNodes.at(0).y;
				node8.z=h+oldNodes.at(0).z;
				nodes.addNode(node8);	
				
				//make new elements
				element bot;
				bot.setNumber(eToRefine);
				vector<int> botNodes;
				botNodes=oldNodeNumbers;
				botNodes.at(4)=node1.nNum;
				botNodes.at(5)=node2.nNum;
				botNodes.at(6)=node3.nNum;
				botNodes.at(7)=node4.nNum;
				bot.setNodes(botNodes);
				elements.replaceElement(bot);
				
				int currentNumberofElements=elements.getHighestElementNumber();
				element rightFlank;
				rightFlank.setNumber(currentNumberofElements+1);
				vector<int> rightFlankNodes;
				rightFlankNodes.resize(8,-1);
				rightFlankNodes.at(0)=oldNodeNumbers.at(0);
				rightFlankNodes.at(1)=oldNodeNumbers.at(1);
				rightFlankNodes.at(2)=node2.nNum;
				rightFlankNodes.at(3)=node1.nNum;
				rightFlankNodes.at(4)=oldNodeNumbers.at(4);
				rightFlankNodes.at(5)=oldNodeNumbers.at(5);
				rightFlankNodes.at(6)=node6.nNum;
				rightFlankNodes.at(7)=node5.nNum;
				rightFlank.setNodes(rightFlankNodes);
				elements.addElement(rightFlank);
				
				element leftFlank;
				leftFlank.setNumber(currentNumberofElements+2);
				vector<int> leftFlankNodes;
				leftFlankNodes.resize(8,-1);
				leftFlankNodes.at(0)=node4.nNum;
				leftFlankNodes.at(1)=node3.nNum;
				leftFlankNodes.at(2)=oldNodeNumbers.at(2);
				leftFlankNodes.at(3)=oldNodeNumbers.at(3);
				leftFlankNodes.at(4)=node8.nNum;
				leftFlankNodes.at(5)=node7.nNum;
				leftFlankNodes.at(6)=oldNodeNumbers.at(6);
				leftFlankNodes.at(7)=oldNodeNumbers.at(7);
				leftFlank.setNodes(leftFlankNodes);
				elements.addElement(leftFlank);
				
				element middle;
				middle.setNumber(currentNumberofElements+3);
				vector<int> middleNodes;
				middleNodes.resize(8,-1);
				middleNodes.at(0)=node1.nNum;
				middleNodes.at(1)=node2.nNum;
				middleNodes.at(2)=node3.nNum;
				middleNodes.at(3)=node4.nNum;
				middleNodes.at(4)=node5.nNum;
				middleNodes.at(5)=node6.nNum;
				middleNodes.at(6)=node7.nNum;
				middleNodes.at(7)=node8.nNum;
				middle.setNodes(middleNodes);
				elements.addElement(middle);
			}
			else if(type==4) //corner element
			{
				//printf("Trying to transmogrify element %d\n",eToRefine);
				element oldElement;
				oldElement=elements.getElement(eToRefine);
				//printf("In method refine element:\n");
				//oldElement.printNodes();
						
				vector<node> oldNodes; // nodes in the element to be refined
				vector<int> oldNodeNumbers; // list of node numbers in the element to be refined
				
				oldNodeNumbers=oldElement.getNodes();
		
				for(int i=0;i<8;i++)
				{	
					node tempNode;			
					int tempNumber=oldNodeNumbers.at(i);
					tempNode=nodes.getNode(tempNumber);
					oldNodes.push_back(tempNode);
				}
				
				//create new nodes
				node node1;
				double h=oldNodes.at(4).z-oldNodes.at(0).z;
				double w=oldNodes.at(1).x-oldNodes.at(0).x;
				double l=oldNodes.at(3).y-oldNodes.at(0).y;

				node1.nNum=nodes.size();
				node1.x=w/2+oldNodes.at(0).x;
				node1.y=l/3+oldNodes.at(0).y;
				node1.z=h/2+oldNodes.at(0).z;
				
				nodes.addNode(node1);
				
				node node2;
				node2.nNum=nodes.size();
				node2.x=w+oldNodes.at(0).x;
				node2.y=l/3+oldNodes.at(0).y;
				node2.z=h/2+oldNodes.at(0).z;
				nodes.addNode(node2);
				
				node node3;
				node3.nNum=nodes.size();
				node3.x=w+oldNodes.at(0).x;
				node3.y=l/3*2+oldNodes.at(0).y;
				node3.z=h/2+oldNodes.at(0).z;
				nodes.addNode(node3);
		
				node node4;
				node4.nNum=nodes.size();
				node4.x=w/2+oldNodes.at(0).x;
				node4.y=l/3*2+oldNodes.at(0).y;
				node4.z=h/2+oldNodes.at(0).z;
				nodes.addNode(node4);		
				
				node node5;
				node5.nNum=nodes.size();
				node5.x=w/2+oldNodes.at(0).x;
				node5.y=l/3+oldNodes.at(0).y;
				node5.z=h+oldNodes.at(0).z;
				nodes.addNode(node5);
				
				node node6;
				node6.nNum=nodes.size();
				node6.x=w+oldNodes.at(0).x;
				node6.y=l/3+oldNodes.at(0).y;
				node6.z=h+oldNodes.at(0).z;
				nodes.addNode(node6);
				
				node node7;
				node7.nNum=nodes.size();
				node7.x=w+oldNodes.at(0).x;
				node7.y=l/3*2+oldNodes.at(0).y;
				node7.z=h+oldNodes.at(0).z;
				nodes.addNode(node7);
		
				node node8;
				node8.nNum=nodes.size();
				node8.x=w/2+oldNodes.at(0).x;
				node8.y=l/3*2+oldNodes.at(0).y;
				node8.z=h+oldNodes.at(0).z;
				nodes.addNode(node8);	
				
				//make new elements
				element bot;
				bot.setNumber(eToRefine);
				vector<int> botNodes;
				botNodes=oldNodeNumbers;
				botNodes.at(4)=node1.nNum;
				botNodes.at(5)=node2.nNum;
				botNodes.at(6)=node3.nNum;
				botNodes.at(7)=node4.nNum;
				bot.setNodes(botNodes);
				elements.replaceElement(bot);
				
				int currentNumberofElements=elements.getHighestElementNumber();
				element rightFlank;
				rightFlank.setNumber(currentNumberofElements+1);
				vector<int> rightFlankNodes;
				rightFlankNodes.resize(8,-1);
				rightFlankNodes.at(0)=oldNodeNumbers.at(0);
				rightFlankNodes.at(1)=oldNodeNumbers.at(1);
				rightFlankNodes.at(2)=node2.nNum;
				rightFlankNodes.at(3)=node1.nNum;
				rightFlankNodes.at(4)=oldNodeNumbers.at(4);
				rightFlankNodes.at(5)=oldNodeNumbers.at(5);
				rightFlankNodes.at(6)=node6.nNum;
				rightFlankNodes.at(7)=node5.nNum;
				rightFlank.setNodes(rightFlankNodes);
				elements.addElement(rightFlank);
				
				element leftFlank;
				leftFlank.setNumber(currentNumberofElements+2);
				vector<int> leftFlankNodes;
				leftFlankNodes.resize(8,-1);
				leftFlankNodes.at(0)=node4.nNum;
				leftFlankNodes.at(1)=node3.nNum;
				leftFlankNodes.at(2)=oldNodeNumbers.at(2);
				leftFlankNodes.at(3)=oldNodeNumbers.at(3);
				leftFlankNodes.at(4)=node8.nNum;
				leftFlankNodes.at(5)=node7.nNum;
				leftFlankNodes.at(6)=oldNodeNumbers.at(6);
				leftFlankNodes.at(7)=oldNodeNumbers.at(7);
				leftFlank.setNodes(leftFlankNodes);
				elements.addElement(leftFlank);
				
				element middle;
				middle.setNumber(currentNumberofElements+3);
				vector<int> middleNodes;
				middleNodes.resize(8,-1);
				middleNodes.at(0)=node1.nNum;
				middleNodes.at(1)=node2.nNum;
				middleNodes.at(2)=node3.nNum;
				middleNodes.at(3)=node4.nNum;
				middleNodes.at(4)=node5.nNum;
				middleNodes.at(5)=node6.nNum;
				middleNodes.at(6)=node7.nNum;
				middleNodes.at(7)=node8.nNum;
				middle.setNodes(middleNodes);
				elements.addElement(middle);
				
				element wall;
				wall.setNumber(currentNumberofElements+4);
				vector<int> wallNodes;
				wallNodes.resize(8,-1);
				wallNodes.at(0)=oldNodeNumbers.at(0);
				wallNodes.at(1)=node1.nNum;
				wallNodes.at(2)=node4.nNum;
				wallNodes.at(3)=oldNodeNumbers.at(3);
				wallNodes.at(4)=oldNodeNumbers.at(4);
				wallNodes.at(5)=node5.nNum;
				wallNodes.at(6)=node8.nNum;
				wallNodes.at(7)=oldNodeNumbers.at(7);
				wall.setNodes(wallNodes);
				elements.addElement(wall);
			}
			else if(type==5)
			{
				//printf("Trying to transmogrify element %d\n",eToRefine);
				element oldElement;
				oldElement=elements.getElement(eToRefine);
				//printf("In method refine element:\n");
				//oldElement.printNodes();
						
				vector<node> oldNodes; // nodes in the element to be refined
				vector<int> oldNodeNumbers; // list of node numbers in the element to be refined
				
				oldNodeNumbers=oldElement.getNodes();
		
				for(int i=0;i<8;i++)
				{	
					node tempNode;			
					int tempNumber=oldNodeNumbers.at(i);
					tempNode=nodes.getNode(tempNumber);
					oldNodes.push_back(tempNode);
				}
				
				//create new nodes
				node node1;
				double h=oldNodes.at(4).z-oldNodes.at(0).z;
				double w=oldNodes.at(1).x-oldNodes.at(0).x;
				double l=oldNodes.at(3).y-oldNodes.at(0).y;

				node1.nNum=nodes.size();
				node1.x=oldNodes.at(0).x;
				node1.y=oldNodes.at(0).y;
				node1.z=h/3+oldNodes.at(0).z;
				
				nodes.addNode(node1);
				
				node node2;
				node2.nNum=nodes.size();
				node2.x=w+oldNodes.at(0).x;
				node2.y=oldNodes.at(0).y;
				node2.z=h/3+oldNodes.at(0).z;
				nodes.addNode(node2);
				
				node node3;
				node3.nNum=nodes.size();
				node3.x=w+oldNodes.at(0).x;
				node3.y=l/2+oldNodes.at(0).y;
				node3.z=h/3+oldNodes.at(0).z;
				nodes.addNode(node3);
		
				node node4;
				node4.nNum=nodes.size();
				node4.x=oldNodes.at(0).x;
				node4.y=l/2+oldNodes.at(0).y;
				node4.z=h/3+oldNodes.at(0).z;
				nodes.addNode(node4);		
				
				node node5;
				node5.nNum=nodes.size();
				node5.x=oldNodes.at(0).x;
				node5.y=oldNodes.at(0).y;
				node5.z=h/3*2+oldNodes.at(0).z;
				nodes.addNode(node5);
				
				node node6;
				node6.nNum=nodes.size();
				node6.x=w+oldNodes.at(0).x;
				node6.y=oldNodes.at(0).y;
				node6.z=h/3*2+oldNodes.at(0).z;
				nodes.addNode(node6);
				
				node node7;
				node7.nNum=nodes.size();
				node7.x=w+oldNodes.at(0).x;
				node7.y=l/2+oldNodes.at(0).y;
				node7.z=h/3*2+oldNodes.at(0).z;
				nodes.addNode(node7);
		
				node node8;
				node8.nNum=nodes.size();
				node8.x=oldNodes.at(0).x;
				node8.y=l/2+oldNodes.at(0).y;
				node8.z=h/3*2+oldNodes.at(0).z;
				nodes.addNode(node8);	
				
				//make new elements
				element bot;
				bot.setNumber(eToRefine);
				vector<int> botNodes;
				botNodes=oldNodeNumbers;
				botNodes.at(0)=node4.nNum;
				botNodes.at(1)=node3.nNum;
				botNodes.at(4)=node8.nNum;
				botNodes.at(5)=node7.nNum;
				bot.setNodes(botNodes);
				elements.replaceElement(bot);
				
				int currentNumberofElements=elements.getHighestElementNumber();
				element rightFlank;
				rightFlank.setNumber(currentNumberofElements+1);
				vector<int> rightFlankNodes;
				rightFlankNodes.resize(8,-1);
				rightFlankNodes.at(0)=oldNodeNumbers.at(0);
				rightFlankNodes.at(1)=oldNodeNumbers.at(1);
				rightFlankNodes.at(2)=oldNodeNumbers.at(2);
				rightFlankNodes.at(3)=oldNodeNumbers.at(3);
				rightFlankNodes.at(4)=node1.nNum;
				rightFlankNodes.at(5)=node2.nNum;
				rightFlankNodes.at(6)=node3.nNum;
				rightFlankNodes.at(7)=node4.nNum;
				rightFlank.setNodes(rightFlankNodes);
				elements.addElement(rightFlank);
				
				element leftFlank;
				leftFlank.setNumber(currentNumberofElements+2);
				vector<int> leftFlankNodes;
				leftFlankNodes.resize(8,-1);
				leftFlankNodes.at(0)=node5.nNum;
				leftFlankNodes.at(1)=node6.nNum;
				leftFlankNodes.at(2)=node7.nNum;
				leftFlankNodes.at(3)=node8.nNum;
				leftFlankNodes.at(4)=oldNodeNumbers.at(4);
				leftFlankNodes.at(5)=oldNodeNumbers.at(5);
				leftFlankNodes.at(6)=oldNodeNumbers.at(6);
				leftFlankNodes.at(7)=oldNodeNumbers.at(7);
				leftFlank.setNodes(leftFlankNodes);
				elements.addElement(leftFlank);
				
				element middle;
				middle.setNumber(currentNumberofElements+3);
				vector<int> middleNodes;
				middleNodes.resize(8,-1);
				middleNodes.at(0)=node1.nNum;
				middleNodes.at(1)=node2.nNum;
				middleNodes.at(2)=node3.nNum;
				middleNodes.at(3)=node4.nNum;
				middleNodes.at(4)=node5.nNum;
				middleNodes.at(5)=node6.nNum;
				middleNodes.at(6)=node7.nNum;
				middleNodes.at(7)=node8.nNum;
				middle.setNodes(middleNodes);
				elements.addElement(middle);
			}
			else if(type==6)
			{
				//printf("Trying to transmogrify element %d\n",eToRefine);
				element oldElement;
				oldElement=elements.getElement(eToRefine);
				//printf("In method refine element:\n");
				//oldElement.printNodes();
						
				vector<node> oldNodes; // nodes in the element to be refined
				vector<int> oldNodeNumbers; // list of node numbers in the element to be refined
				
				oldNodeNumbers=oldElement.getNodes();
		
				for(int i=0;i<8;i++)
				{	
					node tempNode;			
					int tempNumber=oldNodeNumbers.at(i);
					tempNode=nodes.getNode(tempNumber);
					oldNodes.push_back(tempNode);
				}
				
				//create new nodes
				node node1;
				double h=oldNodes.at(4).z-oldNodes.at(0).z;
				double w=oldNodes.at(1).x-oldNodes.at(0).x;
				double l=oldNodes.at(3).y-oldNodes.at(0).y;

				node1.nNum=nodes.size();
				node1.x=w/3+oldNodes.at(0).x;
				node1.y=oldNodes.at(0).y;
				node1.z=h/3+oldNodes.at(0).z;
				
				nodes.addNode(node1);
				
				node node2;
				node2.nNum=nodes.size();
				node2.x=w/3*2+oldNodes.at(0).x;
				node2.y=oldNodes.at(0).y;
				node2.z=oldNodes.at(0).z;
				nodes.addNode(node2);
				
				node node3;
				node3.nNum=nodes.size();
				node3.x=w/3*2+oldNodes.at(0).x;
				node3.y=l/2+oldNodes.at(0).y;
				node3.z=oldNodes.at(0).z;
				nodes.addNode(node3);
		
				node node4;
				node4.nNum=nodes.size();
				node4.x=w/3+oldNodes.at(0).x;
				node4.y=l/2+oldNodes.at(0).y;
				node4.z=oldNodes.at(0).z;
				nodes.addNode(node4);		
				
				node node5;
				node5.nNum=nodes.size();
				node5.x=w/3+oldNodes.at(0).x;
				node5.y=oldNodes.at(0).y;
				node5.z=h+oldNodes.at(0).z;
				nodes.addNode(node5);
				
				node node6;
				node6.nNum=nodes.size();
				node6.x=w/3*2+oldNodes.at(0).x;
				node6.y=oldNodes.at(0).y;
				node6.z=h+oldNodes.at(0).z;
				nodes.addNode(node6);
				
				node node7;
				node7.nNum=nodes.size();
				node7.x=w/3*2+oldNodes.at(0).x;
				node7.y=l/2+oldNodes.at(0).y;
				node7.z=h+oldNodes.at(0).z;
				nodes.addNode(node7);
		
				node node8;
				node8.nNum=nodes.size();
				node8.x=w/3+oldNodes.at(0).x;
				node8.y=l/2+oldNodes.at(0).y;
				node8.z=h+oldNodes.at(0).z;
				nodes.addNode(node8);	
				
				//make new elements
				element bot;
				bot.setNumber(eToRefine);
				vector<int> botNodes;
				botNodes=oldNodeNumbers;
				botNodes.at(0)=node4.nNum;
				botNodes.at(1)=node3.nNum;
				botNodes.at(4)=node8.nNum;
				botNodes.at(5)=node7.nNum;
				bot.setNodes(botNodes);
				elements.replaceElement(bot);
				
				int currentNumberofElements=elements.getHighestElementNumber();
				element rightFlank;
				rightFlank.setNumber(currentNumberofElements+1);
				vector<int> rightFlankNodes;
				rightFlankNodes.resize(8,-1);
				rightFlankNodes.at(0)=oldNodeNumbers.at(0);
				rightFlankNodes.at(1)=node1.nNum;
				rightFlankNodes.at(2)=node4.nNum;
				rightFlankNodes.at(3)=oldNodeNumbers.at(3);
				rightFlankNodes.at(4)=oldNodeNumbers.at(4);
				rightFlankNodes.at(5)=node5.nNum;
				rightFlankNodes.at(6)=node8.nNum;
				rightFlankNodes.at(7)=oldNodeNumbers.at(7);
				rightFlank.setNodes(rightFlankNodes);
				elements.addElement(rightFlank);
				
				element leftFlank;
				leftFlank.setNumber(currentNumberofElements+2);
				vector<int> leftFlankNodes;
				leftFlankNodes.resize(8,-1);
				leftFlankNodes.at(0)=node2.nNum;
				leftFlankNodes.at(1)=oldNodeNumbers.at(1);
				leftFlankNodes.at(2)=oldNodeNumbers.at(2);
				leftFlankNodes.at(3)=node3.nNum;
				leftFlankNodes.at(4)=node6.nNum;
				leftFlankNodes.at(5)=oldNodeNumbers.at(5);
				leftFlankNodes.at(6)=oldNodeNumbers.at(6);
				leftFlankNodes.at(7)=node7.nNum;
				leftFlank.setNodes(leftFlankNodes);
				elements.addElement(leftFlank);
				
				element middle;
				middle.setNumber(currentNumberofElements+3);
				vector<int> middleNodes;
				middleNodes.resize(8,-1);
				middleNodes.at(0)=node1.nNum;
				middleNodes.at(1)=node2.nNum;
				middleNodes.at(2)=node3.nNum;
				middleNodes.at(3)=node4.nNum;
				middleNodes.at(4)=node5.nNum;
				middleNodes.at(5)=node6.nNum;
				middleNodes.at(6)=node7.nNum;
				middleNodes.at(7)=node8.nNum;
				middle.setNodes(middleNodes);
				elements.addElement(middle);
			}
			else if(type==7) //corner element
			{
				//printf("Trying to transmogrify element %d\n",eToRefine);
				element oldElement;
				oldElement=elements.getElement(eToRefine);
				//printf("In method refine element:\n");
				//oldElement.printNodes();
						
				vector<node> oldNodes; // nodes in the element to be refined
				vector<int> oldNodeNumbers; // list of node numbers in the element to be refined
				
				oldNodeNumbers=oldElement.getNodes();
		
				for(int i=0;i<8;i++)
				{	
					node tempNode;			
					int tempNumber=oldNodeNumbers.at(i);
					tempNode=nodes.getNode(tempNumber);
					oldNodes.push_back(tempNode);
				}
				
				//create new nodes
				node node1;
				double h=oldNodes.at(4).z-oldNodes.at(0).z;
				double w=oldNodes.at(1).x-oldNodes.at(0).x;
				double l=oldNodes.at(3).y-oldNodes.at(0).y;

				node1.nNum=nodes.size();
				node1.x=w/2+oldNodes.at(0).x;
				node1.y=oldNodes.at(0).y;
				node1.z=h/3+oldNodes.at(0).z;
				
				nodes.addNode(node1);
				
				node node2;
				node2.nNum=nodes.size();
				node2.x=w+oldNodes.at(0).x;
				node2.y=oldNodes.at(0).y;
				node2.z=h/3+oldNodes.at(0).z;
				nodes.addNode(node2);
				
				node node3;
				node3.nNum=nodes.size();
				node3.x=w+oldNodes.at(0).x;
				node3.y=l/2+oldNodes.at(0).y;
				node3.z=h/3+oldNodes.at(0).z;
				nodes.addNode(node3);
		
				node node4;
				node4.nNum=nodes.size();
				node4.x=w/2+oldNodes.at(0).x;
				node4.y=l/2+oldNodes.at(0).y;
				node4.z=h/3+oldNodes.at(0).z;
				nodes.addNode(node4);		
				
				node node5;
				node5.nNum=nodes.size();
				node5.x=w/2+oldNodes.at(0).x;
				node5.y=oldNodes.at(0).y;
				node5.z=h/3*2+oldNodes.at(0).z;
				nodes.addNode(node5);
				
				node node6;
				node6.nNum=nodes.size();
				node6.x=w+oldNodes.at(0).x;
				node6.y=oldNodes.at(0).y;
				node6.z=h/3*2+oldNodes.at(0).z;
				nodes.addNode(node6);
				
				node node7;
				node7.nNum=nodes.size();
				node7.x=w+oldNodes.at(0).x;
				node7.y=l/2+oldNodes.at(0).y;
				node7.z=h/3*2+oldNodes.at(0).z;
				nodes.addNode(node7);
		
				node node8;
				node8.nNum=nodes.size();
				node8.x=w/2+oldNodes.at(0).x;
				node8.y=l/2+oldNodes.at(0).y;
				node8.z=h/3*2+oldNodes.at(0).z;
				nodes.addNode(node8);	
				
				//make new elements
				element bot;
				bot.setNumber(eToRefine);
				vector<int> botNodes;
				botNodes=oldNodeNumbers;
				botNodes.at(4)=node1.nNum;
				botNodes.at(5)=node2.nNum;
				botNodes.at(6)=node3.nNum;
				botNodes.at(7)=node4.nNum;
				bot.setNodes(botNodes);
				elements.replaceElement(bot);
				
				int currentNumberofElements=elements.getHighestElementNumber();
				element rightFlank;
				rightFlank.setNumber(currentNumberofElements+1);
				vector<int> rightFlankNodes;
				rightFlankNodes.resize(8,-1);
				rightFlankNodes.at(0)=oldNodeNumbers.at(0);
				rightFlankNodes.at(1)=node1.nNum;
				rightFlankNodes.at(2)=node4.nNum;
				rightFlankNodes.at(3)=oldNodeNumbers.at(3);
				rightFlankNodes.at(4)=oldNodeNumbers.at(4);
				rightFlankNodes.at(5)=node5.nNum;
				rightFlankNodes.at(6)=node8.nNum;
				rightFlankNodes.at(7)=oldNodeNumbers.at(7);
				rightFlank.setNodes(rightFlankNodes);
				elements.addElement(rightFlank);
				
				element leftFlank;
				leftFlank.setNumber(currentNumberofElements+2);
				vector<int> leftFlankNodes;
				leftFlankNodes.resize(8,-1);
				leftFlankNodes.at(0)=node4.nNum;
				leftFlankNodes.at(1)=node3.nNum;
				leftFlankNodes.at(2)=oldNodeNumbers.at(2);
				leftFlankNodes.at(3)=oldNodeNumbers.at(3);
				leftFlankNodes.at(4)=node8.nNum;
				leftFlankNodes.at(5)=node7.nNum;
				leftFlankNodes.at(6)=oldNodeNumbers.at(6);
				leftFlankNodes.at(7)=oldNodeNumbers.at(7);
				leftFlank.setNodes(leftFlankNodes);
				elements.addElement(leftFlank);
				
				element middle;
				middle.setNumber(currentNumberofElements+3);
				vector<int> middleNodes;
				middleNodes.resize(8,-1);
				middleNodes.at(0)=node1.nNum;
				middleNodes.at(1)=node2.nNum;
				middleNodes.at(2)=node3.nNum;
				middleNodes.at(3)=node4.nNum;
				middleNodes.at(4)=node5.nNum;
				middleNodes.at(5)=node6.nNum;
				middleNodes.at(6)=node7.nNum;
				middleNodes.at(7)=node8.nNum;
				middle.setNodes(middleNodes);
				elements.addElement(middle);
				
				element wall;
				wall.setNumber(currentNumberofElements+4);
				vector<int> wallNodes;
				wallNodes.resize(8,-1);
				wallNodes.at(0)=node5.nNum;
				wallNodes.at(1)=node6.nNum;
				wallNodes.at(2)=node7.nNum;
				wallNodes.at(3)=node8.nNum;
				wallNodes.at(4)=oldNodeNumbers.at(4);
				wallNodes.at(5)=oldNodeNumbers.at(5);
				wallNodes.at(6)=oldNodeNumbers.at(6);
				wallNodes.at(7)=oldNodeNumbers.at(7);
				wall.setNodes(wallNodes);
				elements.addElement(wall);
			}
			else if(type==8) //corner element NOT FINISHED
			{
				//printf("Trying to transmogrify element %d\n",eToRefine);
				printf("Transmog type VIII is not finished!\n");
				element oldElement;
				oldElement=elements.getElement(eToRefine);
				//printf("In method refine element:\n");
				//oldElement.printNodes();
						
				vector<node> oldNodes; // nodes in the element to be refined
				vector<int> oldNodeNumbers; // list of node numbers in the element to be refined
				
				oldNodeNumbers=oldElement.getNodes();
		
				for(int i=0;i<8;i++)
				{	
					node tempNode;			
					int tempNumber=oldNodeNumbers.at(i);
					tempNode=nodes.getNode(tempNumber);
					oldNodes.push_back(tempNode);
				}
				
				//create new nodes
				node node1;
				double h=oldNodes.at(4).z-oldNodes.at(0).z;
				double w=oldNodes.at(1).x-oldNodes.at(0).x;
				double l=oldNodes.at(3).y-oldNodes.at(0).y;

				node1.nNum=nodes.size();
				node1.x=w/3+oldNodes.at(0).x;
				node1.y=oldNodes.at(0).y;
				node1.z=h/2+oldNodes.at(0).z;
				
				nodes.addNode(node1);
				
				node node2;
				node2.nNum=nodes.size();
				node2.x=w/3*2+oldNodes.at(0).x;
				node2.y=oldNodes.at(0).y;
				node2.z=h/2+oldNodes.at(0).z;
				nodes.addNode(node2);
				
				node node3;
				node3.nNum=nodes.size();
				node3.x=w/3+oldNodes.at(0).x;
				node3.y=l/3+oldNodes.at(0).y;
				node3.z=h/2+oldNodes.at(0).z;
				nodes.addNode(node3);
		
				node node4;
				node4.nNum=nodes.size();
				node4.x=w/3+oldNodes.at(0).x;
				node4.y=l/3*2+oldNodes.at(0).y;
				node4.z=h/2+oldNodes.at(0).z;
				nodes.addNode(node4);		
				
				node node5;
				node5.nNum=nodes.size();
				node5.x=w/3+oldNodes.at(0).x;
				node5.y=oldNodes.at(0).y;
				node5.z=h+oldNodes.at(0).z;
				nodes.addNode(node5);
				
				node node6;
				node6.nNum=nodes.size();
				node6.x=w/3*2+oldNodes.at(0).x;
				node6.y=oldNodes.at(0).y;
				node6.z=h+oldNodes.at(0).z;
				nodes.addNode(node6);
				
				node node7;
				node7.nNum=nodes.size();
				node7.x=w/3*2+oldNodes.at(0).x;
				node7.y=l/3+oldNodes.at(0).y;
				node7.z=h+oldNodes.at(0).z;
				nodes.addNode(node7);
		
				node node8;
				node8.nNum=nodes.size();
				node8.x=w/3+oldNodes.at(0).x;
				node8.y=l/3*2+oldNodes.at(0).y;
				node8.z=h+oldNodes.at(0).z;
				nodes.addNode(node8);	
				
				node node9;
				node9.nNum=nodes.size();
				node9.x=w+oldNodes.at(0).x;
				node9.y=l/3+oldNodes.at(0).y;
				node9.z=h/2+oldNodes.at(0).z;
				nodes.addNode(node9);
				
				node node10;
				node10.nNum=nodes.size();
				node10.x=w+oldNodes.at(0).x;
				node10.y=l/3*2+oldNodes.at(0).y;
				node10.z=h/2+oldNodes.at(0).z;
				nodes.addNode(node10);
				
				node node11;
				node11.nNum=nodes.size();
				node11.x=w+oldNodes.at(0).x;
				node11.y=l/3+oldNodes.at(0).y;
				node11.z=h+oldNodes.at(0).z;
				nodes.addNode(node11);
				
				node node12;
				node12.nNum=nodes.size();
				node12.x=w+oldNodes.at(0).x;
				node12.y=l/3*2+oldNodes.at(0).y;
				node12.z=h+oldNodes.at(0).z;
				nodes.addNode(node12);
				
				//make new elements
				element bot;
				bot.setNumber(eToRefine);
				vector<int> botNodes;
				botNodes=oldNodeNumbers;
				botNodes.at(4)=node1.nNum;
				botNodes.at(5)=node2.nNum;
				botNodes.at(6)=node3.nNum;
				botNodes.at(7)=node4.nNum;
				bot.setNodes(botNodes);
				elements.replaceElement(bot);
				
				int currentNumberofElements=elements.getHighestElementNumber();
				element rightFlank;
				rightFlank.setNumber(currentNumberofElements+1);
				vector<int> rightFlankNodes;
				rightFlankNodes.resize(8,-1);
				rightFlankNodes.at(0)=oldNodeNumbers.at(0);
				rightFlankNodes.at(1)=node1.nNum;
				rightFlankNodes.at(2)=node4.nNum;
				rightFlankNodes.at(3)=oldNodeNumbers.at(3);
				rightFlankNodes.at(4)=oldNodeNumbers.at(4);
				rightFlankNodes.at(5)=node5.nNum;
				rightFlankNodes.at(6)=node8.nNum;
				rightFlankNodes.at(7)=oldNodeNumbers.at(7);
				rightFlank.setNodes(rightFlankNodes);
				elements.addElement(rightFlank);
				
				element leftFlank;
				leftFlank.setNumber(currentNumberofElements+2);
				vector<int> leftFlankNodes;
				leftFlankNodes.resize(8,-1);
				leftFlankNodes.at(0)=node4.nNum;
				leftFlankNodes.at(1)=node3.nNum;
				leftFlankNodes.at(2)=oldNodeNumbers.at(2);
				leftFlankNodes.at(3)=oldNodeNumbers.at(3);
				leftFlankNodes.at(4)=node8.nNum;
				leftFlankNodes.at(5)=node7.nNum;
				leftFlankNodes.at(6)=oldNodeNumbers.at(6);
				leftFlankNodes.at(7)=oldNodeNumbers.at(7);
				leftFlank.setNodes(leftFlankNodes);
				elements.addElement(leftFlank);
				
				element middle;
				middle.setNumber(currentNumberofElements+3);
				vector<int> middleNodes;
				middleNodes.resize(8,-1);
				middleNodes.at(0)=node1.nNum;
				middleNodes.at(1)=node2.nNum;
				middleNodes.at(2)=node3.nNum;
				middleNodes.at(3)=node4.nNum;
				middleNodes.at(4)=node5.nNum;
				middleNodes.at(5)=node6.nNum;
				middleNodes.at(6)=node7.nNum;
				middleNodes.at(7)=node8.nNum;
				middle.setNodes(middleNodes);
				elements.addElement(middle);
				
				element wall;
				wall.setNumber(currentNumberofElements+4);
				vector<int> wallNodes;
				wallNodes.resize(8,-1);
				wallNodes.at(0)=node5.nNum;
				wallNodes.at(1)=node6.nNum;
				wallNodes.at(2)=node7.nNum;
				wallNodes.at(3)=node8.nNum;
				wallNodes.at(4)=oldNodeNumbers.at(4);
				wallNodes.at(5)=oldNodeNumbers.at(5);
				wallNodes.at(6)=oldNodeNumbers.at(6);
				wallNodes.at(7)=oldNodeNumbers.at(7);
				wall.setNodes(wallNodes);
				elements.addElement(wall);
			}
			else
				printf("Type of transmogrification unknown. (Type=%d)\n",type);
		}
		else
		{
			if(type==1)
			{
				printf("Trying to transmogrify element %d\n",eToRefine);
				element oldElement;
				oldElement=elements.getElement(eToRefine);
				//printf("In method refine element:\n");
				//oldElement.printNodes();
						
				vector<node> oldNodes; // nodes in the element to be refined
				vector<int> oldNodeNumbers; // list of node numbers in the element to be refined
				
				oldNodeNumbers=oldElement.getNodes();

				
				for(int i=0;i<oldElement.nodesPerElement;i++)
				{	
					node tempNode;			
					int tempNumber=oldNodeNumbers.at(i);
					tempNode=nodes.getNode(tempNumber);
					oldNodes.push_back(tempNode);
				}
				
				//create new nodes
				node node1;
				double h=oldNodes.at(3).z-oldNodes.at(0).z;
				double w=oldNodes.at(1).x-oldNodes.at(0).x;
				double offset=0;
				node1.nNum=nodes.size();
				node1.x=w/2+oldNodes.at(0).x+offset;
				node1.z=h/3+oldNodes.at(0).z;
				node1.y=oldNodes.at(0).y;
				nodes.addNode(node1);

				node node2;
				node2.nNum=nodes.size();
				node2.x=w+oldNodes.at(0).x+offset;
				node2.z=h/3+oldNodes.at(0).z;
				node2.y=oldNodes.at(0).y;
				nodes.addNode(node2);
				
				node node3;
				node3.nNum=nodes.size();
				node3.x=w+oldNodes.at(0).x+offset;
				node3.z=2*h/3+oldNodes.at(0).z;
				node3.y=oldNodes.at(3).y;
				nodes.addNode(node3);
		
				node node4;
				node4.nNum=nodes.size();
				node4.x=w/2+oldNodes.at(0).x+offset;
				node4.z=2*h/3+oldNodes.at(0).z;
				node4.y=oldNodes.at(3).y;
				nodes.addNode(node4);		
				
				
				//make new elements
				element bot;
				bot.setNumber(eToRefine);
				vector<int> botNodes;
				botNodes=oldNodeNumbers;
				botNodes.at(1)=node1.nNum;
				botNodes.at(2)=node4.nNum;
				bot.setNodes(botNodes);
				elements.replaceElement(bot);
				
				int currentNumberofElements=elements.getHighestElementNumber();
				element rightFlank;
				rightFlank.setNumber(currentNumberofElements+1);
				vector<int> rightFlankNodes;
				rightFlankNodes.resize(4,-1);
				rightFlankNodes.at(0)=oldNodeNumbers.at(0);
				rightFlankNodes.at(1)=oldNodeNumbers.at(1);
				
				
				rightFlankNodes.at(2)=node2.nNum;
				rightFlankNodes.at(3)=node1.nNum;
				
				rightFlank.setNodes(rightFlankNodes);
				elements.addElement(rightFlank);
				
				element leftFlank;
				leftFlank.setNumber(currentNumberofElements+2);
				vector<int> leftFlankNodes;
				leftFlankNodes.resize(4,-1);
				leftFlankNodes.at(0)=node4.nNum;
				leftFlankNodes.at(1)=node3.nNum;
				leftFlankNodes.at(2)=oldNodeNumbers.at(2);
				leftFlankNodes.at(3)=oldNodeNumbers.at(3);
				
				leftFlank.setNodes(leftFlankNodes);
				elements.addElement(leftFlank);
				
				element middle;
				middle.setNumber(currentNumberofElements+3);
				vector<int> middleNodes;
				middleNodes.resize(4,-1);
				middleNodes.at(0)=node1.nNum;
				middleNodes.at(1)=node2.nNum;
				middleNodes.at(2)=node3.nNum;
				middleNodes.at(3)=node4.nNum;

				middle.setNodes(middleNodes);
				elements.addElement(middle);
				
			}
		}
		
		
	}
	
	void transmogElements(vector<int> toBeTrans, int type)
	{
		//int N=(int)toBeTrans.size();
		printf("Transmogrifying %d elements..\n",toBeTrans.size());
		for(int i=0;i<(int)toBeTrans.size();i++)
		{
			transmogElement(toBeTrans.at(i),type);
			//printf("transmoged element %d\n",toBeTrans.at(i));
			//printf("%.2f percent done\n",i/(double)N*100);
		}
	}
	
	void splitElements(vector<int> toBeSplit,int type)
	{
		printf("Splitting %d elements..\n",toBeSplit.size());
		int N=(int)toBeSplit.size();
		for(int i=0;i<N;i++)
		{
			splitElement(toBeSplit.at(i),type);
			//printf("%.2f done\n",i/(double)N*100);
		}
		
	}
	
	void mergeNodes()
	{
		double tolerance=tol;
		// go through all nodes and merge those whos coords coincide
		printf("Merging nodes..."); fflush(stdout);
		int i=0; int size=nodes.nodes.size();
		while(1)
		{
			//double quota=i/(double)size*100;
			//printf("Merging nodes, %.2f percent done.\n",quota);
			
			size=nodes.nodes.size();
			if(i>=size)
			{
				break;
			}
			node theNode;
			theNode=nodes.nodes.at(i);
			double x=theNode.x;
			double y=theNode.y;
			double z=theNode.z;
			vector<int> nodesAtTheSameLocation;
			nodesAtTheSameLocation=nodes.getNodesAt(x,y,z,tolerance); // does include "theNode"
			if(nodesAtTheSameLocation.size()>1) //if there is more than 1 node at the same coord
			{

				elements.replaceNodes(nodesAtTheSameLocation,theNode.nNum);

				for(int j=0;j<(int)nodesAtTheSameLocation.size();j++) //delete the replaced nodes
				{
					if(theNode.nNum!=nodesAtTheSameLocation.at(j))
					{
						nodes.deleteNode(nodesAtTheSameLocation.at(j));
					}
				}
			}
			i++;
		}
		nodes.checkMap();
		printf(" done.\n"); fflush(stdout);
	}
	
	vector<int> getElementsAtX(double x_coord,double tol) 
	{
		vector<int> theElements;
		theElements=elements.getElementsAt(x_coord,x_coord,-100000,100000,-100000,100000);
		return theElements;
	}
	
	vector<int> getElementsAt(double xl,double xh,double yl,double yh,double zl,double zh)
	{
		vector<int> theElements;
		theElements=elements.getElementsAt(xl,xh,yl,yh,zl,zh); //get all elements that has any of these nodes in them
		return theElements;
	}
	
	vector<int> getElementsToTheRightOf(double x_coord) 
	{
		vector<int> theElements;
		theElements=elements.getElementsToTheRightOf(x_coord,nodes);
		return theElements;
	}
	
	vector<int> eselY(double y_coord)
	{
		return elements.eselY(y_coord,nodes);
	}
	
	vector<int> esln(vector<int> nodes, int type) // get elements within the nodeset
	{
		return elements.getElementsWithNodes(nodes,type);
	}
	
	void replicate()
	{
		printf("Replicating elementary model...\n");
		nodeBox newNodes;
		elementBox newElements;
		
		newNodes=nodes;
		newElements=elements;
		
		int numberOfNodes=nodes.getHighestNodeNumber();
		int numberOfElements=elements.getHighestElementNumber();
		
		//for loop
		for(int i=0;i<reps-1;i++)
		{
			//printf("Shifting numbers\n");
			newNodes.shiftNumbers(numberOfNodes);
			//printf("Translating nodes\n");
			newNodes.translateAllNodes(0,length,0);
			//printf("Adding nodes\n");
			nodes.addNodes(newNodes.getNodes());
			
			//printf("Shifting element numbers\n");
			newElements.shiftElementNumbers(numberOfElements);
			//printf("Shifting node numbers in elements\n");
			newElements.shiftNodeNumbers(numberOfNodes);
			//printf("Adding elements.\n");
			elements.addElements(newElements.getElements());
			printf("Done with rep %d of %d\n",i+2,reps);
		}
		printf("Done replicating.\n");
	}
	void createDCB()
	{
		if(dcb)
		{
			printf("Creating DCB...\n");
			nodeBox newNodes;
			elementBox newElements;
			
			newNodes=nodes;
			newElements=elements;
			
			int numberOfNodes=nodes.getHighestNodeNumber();
			int numberOfElements=elements.getHighestElementNumber();
		
			printf("Shifting numbers %d\n",numberOfNodes);
			newNodes.shiftNumbers(numberOfNodes);
			//printf("Translating nodes\n");
			newNodes.translateAllNodes(0,0,-thickness-cohesiveThickness);
			//printf("Adding nodes\n");
			nodes.addNodes(newNodes.getNodes());
			
			//printf("Shifting element numbers\n");
			newElements.shiftElementNumbers(numberOfElements);
			//printf("Shifting node numbers in elements\n");
			newElements.shiftNodeNumbers(numberOfNodes);
			//printf("Adding elements.\n");
			elements.addElements(newElements.getElements());
		}
	}

	void mirror()
	{
		if(toMirror)
		{
			printf("Mirroring mesh...\n");
			nodeBox newNodes;
			elementBox newElements;
			
			newNodes=nodes;
			newElements=elements;
			
			int numberOfNodes=nodes.getHighestNodeNumber();
			int numberOfElements=elements.getHighestElementNumber();
		
			printf("Shifting numbers %d\n",numberOfNodes);
			newNodes.shiftNumbers(numberOfNodes);
			//printf("Translating nodes\n");
			newNodes.changeXsign();
			//printf("Adding nodes\n");
			nodes.addNodes(newNodes.getNodes());
			
			//printf("Shifting element numbers\n");
			newElements.shiftElementNumbers(numberOfElements);
			//printf("Shifting node numbers in elements\n");
			newElements.shiftNodeNumbers(numberOfNodes);
			//printf("Adding elements.\n");
			newElements.mirror(); //change the numbering
			elements.addElements(newElements.getElements());
		}
        /*
		if(toMirror) //temporary (to mesh in Z direction)
		{
			printf("Mirroring mesh...\n");
			nodeBox newNodes;
			elementBox newElements;
			
			newNodes=nodes;
			newElements=elements;
			
			int numberOfNodes=nodes.getHighestNodeNumber();
			int numberOfElements=elements.getHighestElementNumber();
		
			printf("Shifting numbers %d\n",numberOfNodes);
			newNodes.shiftNumbers(numberOfNodes);
			//printf("Translating nodes\n");
			newNodes.changeZsign();
			//printf("Adding nodes\n");
			nodes.addNodes(newNodes.getNodes());
			
			//printf("Shifting element numbers\n");
			newElements.shiftElementNumbers(numberOfElements);
			//printf("Shifting node numbers in elements\n");
			newElements.shiftNodeNumbers(numberOfNodes);
			//printf("Adding elements.\n");
			newElements.mirror(); //change the numbering
			elements.addElements(newElements.getElements());
		}*/
        
	}
	void createCohesiveLayer()
	{
		if(cohesiveThickness>0)
			{
			printf("Creating cohesive layer...\n");
			
			nodeBox newNodes;
			double cracktip=length*cohesiveFraction*reps; //where the cohesive layers starts 
			bottomNodes=nodes.nselXYZ(-width-tol,0+tol,0-tol,cracktip+tol,-thickness-tol,-thickness+tol);
			printf("bottom nodes in cohesive: %d\n",bottomNodes.size());
			
			for(unsigned int i=0;i<bottomNodes.size();i++)
			{
				newNodes.addNode(nodes.getNode(bottomNodes.at(i)));
			}
			int numberOfNodes=nodes.getHighestNodeNumber();
			newNodes.shiftNumbers(numberOfNodes);
			newNodes.translateAllNodes(0,0,-cohesiveThickness);
			//printf("bottomNodes in cohesive: %d\n",bottomNodes.size());
			//printf("new nodes in cohesive: %d\n",newNodes.size());
			//printf("cohesive nodes in cohesive: %d\n",cohesiveNodes.size());
			nodes.addNodes(newNodes.getNodes());
			//take bottom layer of nodes and copy
			//translate the copies
			
			vector<int>	bottomElementsNumbers;
			bottomElementsNumbers=elements.getElementsWithNodes(bottomNodes,4); //returns all the elements with 4 nodes in the set bottomNodes
			printf("bottomElementsNumbers.size() = %d\n",bottomElementsNumbers.size());
			element currentElement;
			vector<int> currentNodes;
			for(unsigned int i=0;i<bottomElementsNumbers.size();i++)
			{
				currentElement=elements.getElement(bottomElementsNumbers.at(i));
				currentNodes=currentElement.getNodes();
				currentNodes.at(4)=currentNodes.at(0);
				currentNodes.at(5)=currentNodes.at(1);
				currentNodes.at(6)=currentNodes.at(2);
				currentNodes.at(7)=currentNodes.at(3);
				currentNodes.at(0)=currentNodes.at(0)+numberOfNodes;
				currentNodes.at(1)=currentNodes.at(1)+numberOfNodes;
				currentNodes.at(2)=currentNodes.at(2)+numberOfNodes;
				currentNodes.at(3)=currentNodes.at(3)+numberOfNodes;
				currentElement.setNodes(currentNodes);
				currentElement.setNumber(i+elements.getHighestElementNumber()+1);
				elements.addElement(currentElement);
			}
			//go through all the bottom elements
			//create new elements that have the nodes of the bottom elements as top nodes and the new nodes as bottom nodes
		}

	}
	void shrinkCohesiveLayer() //after all nodes have been merged shrink the cohesive layer (only for flat cohesive layer)
	{
		if(flatCohesiveLayer)
		{
			printf("Shrinking the cohesive layer to nothing\n");
			//translate the lower portion of "block" in the z direction
			vector<int> lowerNodes=nodes.nselXYZ(-width-tol,tol,0-tol,length+tol,-thickness*2-cohesiveThickness-tol,-thickness-cohesiveThickness+tol);
			printf("lower nodes (z=%f..%f): %d\n",-thickness*2-cohesiveThickness-tol,-thickness-cohesiveThickness+tol,lowerNodes.size());
			for(unsigned int i=0;i<lowerNodes.size();i++)
			{
				nodes.translateNode(lowerNodes.at(i),0,0,cohesiveThickness);
			}
			cohesiveThickness=0;
		}
	}
	
	void makeSets()
	{
		double cracktip=length*reps*cohesiveFraction; //where the cohesive layers starts 
		nodeNumbers=nodes.getNodeNumbers();
		cohesiveNodes=nodes.nselXYZ(-width-tol,0+tol,0-tol,cracktip+tol,-thickness-tol-cohesiveThickness,-thickness+tol-cohesiveThickness);
		bottomNodes=nodes.nselXYZ(-width-tol,0+tol,0-tol,cracktip+tol,-thickness-tol,-thickness+tol);
		cohesiveElements=elements.getElementsWithNodes(add(cohesiveNodes,bottomNodes),1);
		inletLower=nodes.nselXYZ(-width,0,length*reps,length*reps,-thickness*2-cohesiveThickness,-thickness-cohesiveThickness);
		inletUpper=nodes.nselXYZ(-width,0,length*reps,length*reps,-thickness,0);
		blockElements=subtract(elements.getNumbers(),cohesiveElements);
		blockNodes=nodes.nselXYZ(-width-tol,0+tol,0-tol,length*reps+tol,-thickness-tol,0+tol);
	}

	void stretch(double start, double stop) //stretch mesh in z and x directions. (Mostly for AM)
	{
		vector<int> nodelist=nodes.getListOfNodes();

		

		//printf("xscale %f\n",xscale);
		//iterate over nodes
		for(unsigned int i=0; i<nodelist.size(); i++) // goes through all nodes
		{
			int nNum=nodelist[i];
			node n = nodes.getNode(nNum);
			//if(n.x<-start or n.z<-start)
			//{
				double xscale=stop/width;
				double zscale=stop/thickness;
				double xtrans=xscale*(n.x+start)-n.x;
				double ztrans=zscale*(n.z+start)-n.z;
				nodes.translateNode(nNum,xtrans,0,ztrans);
				//printf("x,xtrans %f,%f\n",n.x,xtrans);
			//}
		}

	}

	void flatten(double factor)
	{
		vector<int> nodelist=nodes.getListOfNodes();
		for(unsigned int i=0; i<nodelist.size(); i++) // goes through all nodes
		{
			
			int nNum=nodelist[i];
			node n = nodes.getNode(nNum);

			double newPos=n.y*factor;
			double ytrans=newPos-n.y;
			nodes.translateNode(nNum,0,ytrans,0);
		}
	}
	
};

mesh parseInput(char inputFile[]);
void writeMesh(mesh theMesh);


int main(int argc, char* argv[]) //the program starts here
{
	printf("**************************************\n");
	printf("* Thinmesh [Fredrik Wredenberg 2007] *\n");
	printf("**************************************\n");
	
	if(argc>0)
	{
		mesh myMesh=parseInput(argv[1]);
		myMesh.generateBaseMesh();
		fflush(stdout);
		myMesh.refineMesh();
		fflush(stdout);
		myMesh.createDCB(); //mirrors the block mesh to form a DCB
		myMesh.mergeNodes();
		myMesh.mirror();
		fflush(stdout);
		//myMesh.createCohesiveLayer(); //creates the cohesive layer
		//fflush(stdout);
		myMesh.mergeNodes(); // all incident are merged into one
		/*
		myMesh.replicate();
		fflush(stdout);
		myMesh.mergeNodes(); // all incident are merged into one
		fflush(stdout);
		*/
		myMesh.makeSets();
		//myMesh.shrinkCohesiveLayer();
		
		fflush(stdout);
		//myMesh.stretch(0.1,2.5);	
		//myMesh.flatten(0.066000066);	
		writeMesh(myMesh);
	}
	else
	{
		printf("Please specify input file.\n");
	}
	
	return 0;
}	

mesh parseInput(char inputFile[]) //parse the input file to a model
{
	char line[200];
	char search[200];
	char widthMarker[] = {"*width"};
	char lengthMarker[] = {"*length"};
	char thicknessMarker[] = {"*thickness"};
	char divMarker[] = {"*divisions"};
	char refineMarker[] = {"*refine"};
	char repsMarker[]= {"*reps"};
	char cohesiveMarker[] = {"*cohesive"};
	char substrateMarker[] = {"*substrate"};
	char dcbMarker[] = {"*dcb"};
	char mirrorMarker[] = {"*mirror"};

	FILE *infile;
	infile = fopen(inputFile,"r");

	mesh theMesh;
	
	while( !feof( infile ) )
	{
		fgets(line,199,infile);
		sscanf(line,"%s",search);
		//printf("%s",line);
		if (strcmp(search,widthMarker)==0)
		{
			float width;
			fgets(line,199,infile);
			sscanf(line,"%f\n",&width);
			printf("width: %f\n",width);
			theMesh.setWidth(width);
		}
		if (strcmp(search,lengthMarker)==0)
		{
			float length;
			fgets(line,199,infile);
			sscanf(line,"%f\n",&length);
			printf("length: %f\n",length);
			theMesh.setLength(length);
		}
		if (strcmp(search,thicknessMarker)==0)
		{
			float thickness;
			fgets(line,199,infile);
			sscanf(line,"%f\n",&thickness);
			printf("thickness: %f\n",thickness);
			theMesh.setThickness(thickness);
		}
		if (strcmp(search,divMarker)==0)
		{
			int div;
			fgets(line,199,infile);
			sscanf(line,"%d\n",&div);
			printf("div: %d\n",div);
			theMesh.setDivisions(div);
		}
		if (strcmp(search,repsMarker)==0)
		{
			int reps;
			fgets(line,199,infile);
			sscanf(line,"%d\n",&reps);
			printf("reps: %d\n",reps);
			theMesh.setReps(reps);
		}
		if (strcmp(search,cohesiveMarker)==0)
		{
			float cthick;
			fgets(line,199,infile);
			sscanf(line,"%f\n",&cthick);
			printf("cohesive layer thickness: %f\n",cthick);
			theMesh.setCohesiveThickness(cthick);
		}
		if (strcmp(search,substrateMarker)==0)
		{
			float sthick;
			fgets(line,199,infile);
			sscanf(line,"%f\n",&sthick);
			printf("substrate thickness: %f\n",sthick);
			theMesh.setSubstrateThickness(sthick);
		}
		if (strcmp(search,dcbMarker)==0)
		{
			float cohesiveFraction;
			fgets(line,199,infile);
			sscanf(line,"%f\n",&cohesiveFraction);
			printf("DCB model: cohesive fraction: %f\n",cohesiveFraction);
			theMesh.setCohesiveFraction(cohesiveFraction);
			theMesh.toggleDCB();
		}
		if (strcmp(search,refineMarker)==0)
		{
			float x,z;
			refPoint ref;
			int type;
			
			vector<refPoint> refs;

			printf("Lines of refinement x,z,type=");
			while(1)
			{
				fgets(line,199,infile);
				if(feof(infile))
					break;
				sscanf(line,"%f,%f,%d\n",&x,&z,&type);
				printf("%f,%f,%d ",x,z,type);
				ref.x=x;
				ref.z=z;
				ref.type=type;
				refs.push_back(ref);
			}
			printf("\n");
			theMesh.setRefinement(refs); 
		}
		
		if (strcmp(search,mirrorMarker)==0)
		{
			theMesh.toggleMirror();
		}
	}

	fclose(infile);
	return theMesh;
}

void writeMesh(mesh theMesh)
{
	printf("Writing mesh\n");
	FILE *outFile;
	outFile = fopen("thinmesh.inp","w");

//	fprintf(outFile,"*NODE,NSET=block\n");
	vector<node> nodes;
	nodes=theMesh.nodes.getNodes();
	vector<element> elements;
	elements=theMesh.elements.getElements();
	
//	for(unsigned int i=0;i<nodes.size();i++)
//	{
//		int num=nodes.at(i).nNum;
//		double x=nodes.at(i).x;
//		double y=nodes.at(i).y;
//		double z=nodes.at(i).z;
//		fprintf(outFile,"%d,%f,%f,%f\n",num,x,y,z);
//	}
//	
//	fprintf(outFile,"*ELEMENT, TYPE=C3D8R, ELSET=block\n");
//	for( unsigned int i=0;i<elements.size();i++)
//	{
//		int num=elements.at(i).eNum;
//		vector<int> nodes=elements.at(i).getNodes();
//		fprintf(outFile,"%d,%d,%d,%d,%d,%d,%d,%d,%d\n",num,nodes.at(0),nodes.at(1),nodes.at(2),nodes.at(3),nodes.at(4),nodes.at(5),nodes.at(6),nodes.at(7));		
//	}
//	fclose(outFile);
	
	outFile = fopen("nodes.dat","w"); // for meshPlot in octave
	for( unsigned int i=0;i<nodes.size();i++)
	{
		int num=nodes.at(i).nNum;
		double x=nodes.at(i).x;
		double y=nodes.at(i).y;
		double z=nodes.at(i).z;
		fprintf(outFile,"%d,%f,%f,%f\n",num,x,y,z);
	}
	fclose(outFile);

	outFile = fopen("elements.dat","w"); // for meshPlot in octave
	if(theMesh.ldiv==0){fprintf(outFile,"*ELEMENT,TYPE=CPE4,ELSET=ALL\n");}
	else{fprintf(outFile,"*ELEMENT,TYPE=C3D8,ELSET=ALL\n");}
	for(unsigned int i=0;i<elements.size();i++)
	{
		int num=elements.at(i).eNum;
		vector<int> nodes=elements.at(i).getNodes();
		//printf("Nodes per element %d\n",nodes.size());
		if(nodes.size()==4)
		{
			fprintf(outFile,"%d,%d,%d,%d,%d\n",num,nodes.at(0),nodes.at(1),nodes.at(2),nodes.at(3));		
		}
		else if (nodes.size()==8)
		{
			fprintf(outFile,"%d,%d,%d,%d,%d,%d,%d,%d,%d\n",num,nodes.at(0),nodes.at(1),nodes.at(2),nodes.at(3),nodes.at(4),nodes.at(5),nodes.at(6),nodes.at(7));		
		}
		else
		{
			printf("Error. Elements with %d nodes not supported",nodes.size());
		}
		
	}
	fclose(outFile);
// 	/*
// 	// below is for ansys2abaqus
 	vector<int> selNodes; 
 	vector<int> selElements;
	
// 	outFile = fopen("NLIST.lis","w");
// 	selNodes=theMesh.blockNodes;
// 	//selNodes=theMesh.nodes.nselXYZ(-theMesh.width,0,0,theMesh.length,-theMesh.thickness,0);
// 	for( unsigned int i=0;i<selNodes.size();i++)
// 	{
// 		int num=selNodes.at(i);
// 		double x=theMesh.nodes.getNode(num).x;
// 		double y=theMesh.nodes.getNode(num).y;
// 		double z=theMesh.nodes.getNode(num).z;
// 		fprintf(outFile,"%d %f %f %f 0 0 0\n",num,x,y,z);
// 	}
// 	/*selNodes=theMesh.nodes.nselXYZ(-theMesh.width,0,0,theMesh.length,-theMesh.thickness*2-theMesh.cohesiveThickness,-theMesh.thickness-theMesh.cohesiveThickness);
// 	for( unsigned int i=0;i<selNodes.size();i++)
// 	{
// 		int num=selNodes.at(i);
// 		double x=theMesh.nodes.getNode(num).x;
// 		double y=theMesh.nodes.getNode(num).y;
// 		double z=theMesh.nodes.getNode(num).z;
// 		fprintf(outFile,"%d %f %f %f 0 0 0\n",num,x,y,z);
// 	}*/
// 	fclose(outFile);
	
	
// 	outFile = fopen("ELIST.lis","w");
// //	selNodes=theMesh.nodes.nselXYZ(-theMesh.width,0,0,theMesh.length,-theMesh.thickness,0);
// //	selElements=theMesh.elements.getElementsWithNodes(selNodes,1);
// //	for( unsigned int i=0;i<selElements.size();i++)
// //	{
// //		int num=selElements.at(i);
// //		vector<int> nodes=theMesh.elements.getElement(num).getNodes();
// //		fprintf(outFile,"%d 0 0 0 0 0 %d %d %d %d %d %d %d %d\n",num,nodes.at(0),nodes.at(1),nodes.at(2),nodes.at(3),nodes.at(4),nodes.at(5),nodes.at(6),nodes.at(7));		
// //	}
// //	selNodes=theMesh.nodes.nselXYZ(-theMesh.width,0,0,theMesh.length,-theMesh.thickness*2-theMesh.cohesiveThickness,-theMesh.thickness-theMesh.cohesiveThickness);
// //	selElements=theMesh.elements.getElementsWithNodes(selNodes,1);
// //	for( unsigned int i=0;i<selElements.size();i++)
// //	{
// //		int num=selElements.at(i);
// //		vector<int> nodes=theMesh.elements.getElement(num).getNodes();
// //		fprintf(outFile,"%d 0 0 0 0 0 %d %d %d %d %d %d %d %d\n",num,nodes.at(0),nodes.at(1),nodes.at(2),nodes.at(3),nodes.at(4),nodes.at(5),nodes.at(6),nodes.at(7));		
// //	}
// 	selElements=theMesh.blockElements;
// 	for( unsigned int i=0;i<selElements.size();i++)
// 	{
// 		int num=selElements.at(i);
// 		vector<int> nodes=theMesh.elements.getElement(num).getNodes();
// 		fprintf(outFile,"%d 0 0 0 0 0 %d %d %d %d %d %d %d %d\n",num,nodes.at(0),nodes.at(1),nodes.at(2),nodes.at(3),nodes.at(4),nodes.at(5),nodes.at(6),nodes.at(7));		
// 	}
// 	fclose(outFile);
	
// 	//printf("debug 1\n");
// 	outFile = fopen("cohesive.nod","w");
// 	//vector<int> cohesiveNodes=theMesh.nodes.nselXYZ(-theMesh.width,0,0,theMesh.length,-theMesh.thickness-theMesh.cohesiveThickness,-theMesh.thickness-theMesh.cohesiveThickness);
// 	vector<int> cohesiveNodes=theMesh.cohesiveNodes;
// 	for( unsigned int i=0;i<cohesiveNodes.size();i++)
// 	{
// 		int num=cohesiveNodes.at(i);
// 		//printf("num=%d\n",num);
// 		double x=theMesh.nodes.getNode(num).x;
// 		double y=theMesh.nodes.getNode(num).y;
// 		double z=theMesh.nodes.getNode(num).z;
// 		fprintf(outFile,"%d %f %f %f 0 0 0\n",num,x,y,z);
// 		//printf("%d %f %f %f 0 0 0\n",num,x,y,z);
// 	}
// 	fclose(outFile);
	
// 	//printf("debug 2\n");
// 	outFile = fopen("bottom.nod","w");
// 	vector<int> bottomNodes=theMesh.bottomNodes;
// 	for( unsigned int i=0;i<bottomNodes.size();i++)
// 		fprintf(outFile,"%d 0 0 0 0 0 0\n",bottomNodes.at(i));
// 	fclose(outFile);
	
// 	outFile = fopen("cohesive.ele","w");
// 	printf("cohesive elements:\n");
// 	vector<int> coAndBo=add(cohesiveNodes,bottomNodes);
// 	selElements=theMesh.elements.getElementsWithNodes(coAndBo,1); 
// 	//selElements=theMesh.getElementsAt(-theMesh.width,0,0,theMesh.length,-theMesh.thickness-theMesh.cohesiveThickness,-theMesh.thickness);
// 	for( unsigned int i=0;i<selElements.size();i++)
// 	{
// 		int num=selElements.at(i);
// 		vector<int> nodes=theMesh.elements.getElement(num).getNodes();
// 		fprintf(outFile,"%d 0 0 0 0 0 %d %d %d %d %d %d %d %d\n",num,nodes.at(0),nodes.at(1),nodes.at(2),nodes.at(3),nodes.at(4),nodes.at(5),nodes.at(6),nodes.at(7));		
// 	}
// 	fclose(outFile);
	
// 	outFile = fopen("substrate.nod","w");
// 	selNodes=theMesh.nodes.nselXYZ(-theMesh.width,0,0,theMesh.length*theMesh.reps,-theMesh.thickness,-theMesh.thickness+theMesh.substrateThickness);
// 	for( unsigned int i=0;i<selNodes.size();i++)
// 	{
// 		int num=selNodes.at(i);
// 		double x=theMesh.nodes.getNode(num).x;
// 		double y=theMesh.nodes.getNode(num).y;
// 		double z=theMesh.nodes.getNode(num).z;
// 		fprintf(outFile,"%d %f %f %f 0 0 0\n",num,x,y,z);
// 	}
// 	fclose(outFile);
	
	
// 	outFile = fopen("substrate.ele","w");
// 	selElements=theMesh.elements.getElementsWithNodes(selNodes,1);
// 	for( unsigned int i=0;i<selElements.size();i++)
// 	{
// 		int num=selElements.at(i);
// 		vector<int> nodes=theMesh.elements.getElement(num).getNodes();
// 		fprintf(outFile,"%d 0 0 0 0 0 %d %d %d %d %d %d %d %d\n",num,nodes.at(0),nodes.at(1),nodes.at(2),nodes.at(3),nodes.at(4),nodes.at(5),nodes.at(6),nodes.at(7));		
// 	}
// 	fclose(outFile);
	
// 	outFile = fopen("film.nod","w");
// 	selNodes=subtract(theMesh.nodes.getListOfNodes(),selNodes); //all_nodes - substrate
// 	for( unsigned int i=0;i<selNodes.size();i++)
// 	{
// 		int num=selNodes.at(i);
// 		double x=theMesh.nodes.getNode(num).x;
// 		double y=theMesh.nodes.getNode(num).y;
// 		double z=theMesh.nodes.getNode(num).z;
// 		fprintf(outFile,"%d %f %f %f 0 0 0\n",num,x,y,z);
// 	}
// 	fclose(outFile);
	
	
// 	outFile = fopen("film.ele","w");
// 	selElements=theMesh.elements.getElementsWithNodes(selNodes,0);
// 	for( unsigned int i=0;i<selElements.size();i++)
// 	{
// 		int num=selElements.at(i);
// 		vector<int> nodes=theMesh.elements.getElement(num).getNodes();
// 		fprintf(outFile,"%d 0 0 0 0 0 %d %d %d %d %d %d %d %d\n",num,nodes.at(0),nodes.at(1),nodes.at(2),nodes.at(3),nodes.at(4),nodes.at(5),nodes.at(6),nodes.at(7));		
// 	}
// 	fclose(outFile);
	
// 	outFile = fopen("top.nod","w");
// 	selNodes=theMesh.nodes.nselZ(0, theMesh.tol);
// 	for( unsigned int i=0;i<selNodes.size();i++)
// 		fprintf(outFile,"%d 0 0 0 0 0 0\n",selNodes.at(i));
// 	fclose(outFile);
	
// 	outFile = fopen("inside.nod","w");
// 	selNodes=theMesh.nodes.nselX(0,0, theMesh.tol);
// 	for( unsigned int i=0;i<selNodes.size();i++)
// 		fprintf(outFile,"%d 0 0 0 0 0 0\n",selNodes.at(i));
// 	fclose(outFile);
	
// 	outFile = fopen("outside.nod","w");
// 	selNodes=theMesh.nodes.nselX(-theMesh.width, -theMesh.width, theMesh.tol);
// 	for( unsigned int i=0;i<selNodes.size();i++)
// 		fprintf(outFile,"%d 0 0 0 0 0 0\n",selNodes.at(i));
// 	fclose(outFile);
	
// 	outFile = fopen("inlet.nod","w");
// 	selNodes=theMesh.nodes.nselY(theMesh.length*theMesh.reps,theMesh.length*theMesh.reps, theMesh.tol);
// 	for( unsigned int i=0;i<selNodes.size();i++)
// 		fprintf(outFile,"%d 0 0 0 0 0 0\n",selNodes.at(i));
// 	fclose(outFile);
	
// 	outFile = fopen("inletu.nod","w");
// 	vector<int>  lowerNodes;
// //	lowerNodes=theMesh.nodes.nselZ(-theMesh.thickness-theMesh.cohesiveThickness,0, theMesh.tol);
// 	selNodes=theMesh.inletUpper;
// //	selNodes=subtract(selNodes,lowerNodes);
// 	for( unsigned int i=0;i<selNodes.size();i++)
// 		fprintf(outFile,"%d 0 0 0 0 0 0\n",selNodes.at(i));
// 	fclose(outFile);
	
// 	outFile = fopen("inletl.nod","w");
// 	vector<int>  upperNodes;
// 	selNodes=theMesh.inletLower;
// 	for( unsigned int i=0;i<selNodes.size();i++)
// 		fprintf(outFile,"%d 0 0 0 0 0 0\n",selNodes.at(i));
// 	fclose(outFile);
	
// 	outFile = fopen("outlet.nod","w");
// 	selNodes=theMesh.nodes.nselY(0,0, theMesh.tol);
// 	for( unsigned int i=0;i<selNodes.size();i++)
// 		fprintf(outFile,"%d 0 0 0 0 0 0\n",selNodes.at(i));
// 	fclose(outFile);
	
// 	double tol=theMesh.width/theMesh.wdiv/1000;
// 	outFile = fopen("inletNodesSmall.nod","w");
// 	selNodes=theMesh.nodes.nselXYZ(-theMesh.width-tol,-tol,theMesh.length*theMesh.reps,theMesh.length*theMesh.reps,-theMesh.thickness+tol,tol);
// 	for( unsigned int i=0;i<selNodes.size();i++)
// 		fprintf(outFile,"%d 0 0 0 0 0 0\n",selNodes.at(i));
// 	fclose(outFile);
	
// 	outFile = fopen("inletSmall1.nod","w");
// 	selNodes=theMesh.nodes.nselXYZ(-theMesh.width,-tol,theMesh.length*theMesh.reps,theMesh.length*theMesh.reps,-theMesh.thickness,0);
// 	for( unsigned int i=0;i<selNodes.size();i++)
// 		fprintf(outFile,"%d 0 0 0 0 0 0\n",selNodes.at(i));
// 	fclose(outFile);
	
// 	outFile = fopen("inletSmall2.nod","w");
// 	selNodes=theMesh.nodes.nselXYZ(-theMesh.width,0,theMesh.length*theMesh.reps,theMesh.length*theMesh.reps,-theMesh.thickness+tol,0);
// 	for( unsigned int i=0;i<selNodes.size();i++)
// 		fprintf(outFile,"%d 0 0 0 0 0 0\n",selNodes.at(i));
// 	fclose(outFile);
	
// 	outFile = fopen("allButInAndOut.nod","w");
// 	selNodes=theMesh.nodes.nselXYZ(-theMesh.width,0.0,0.0,theMesh.length*theMesh.reps-tol,-theMesh.thickness,-theMesh.thickness);
// 	for( unsigned int i=0;i<selNodes.size();i++)
// 		fprintf(outFile,"%d 0 0 0 0 0 0\n",selNodes.at(i));
// 	fclose(outFile);
	
// //	outFile = fopen("inletSmall1.nod","w"); //not really the inlet but the internals
// //	selNodes=theMesh.nodes.nselXYZ(theMesh.linesOfRefinement.at(0)+theMesh.width/theMesh.wdiv+0.0000001,-0.0000001,0.0000001,theMesh.length-0.0000001,-theMesh.thickness+0.0000001,-0.0000001);
// //	for( unsigned int i=0;i<selNodes.size();i++)
// //		fprintf(outFile,"%d 0 0 0 0 0 0\n",selNodes.at(i));
// //	fclose(outFile);
	
// 	outFile = fopen("tip.ele","w");
// 	//selNodes=theMesh.nodes.nselX(theMesh.refPoints.at(0).x,1000000, theMesh.tol);
// 	selNodes=theMesh.nodes.nselXYZ(-theMesh.thickness*2-theMesh.tol,theMesh.tol,0-theMesh.tol,theMesh.length*theMesh.reps+theMesh.tol,-theMesh.thickness/2-theMesh.tol,theMesh.tol);
// 	selElements=theMesh.elements.getElementsWithNodes(selNodes,1);
// 	for( unsigned int i=0;i<selElements.size();i++)
// 		fprintf(outFile,"%d 0 0 0 0 0 0 0 0 0 0 0 0 0 0\n",selElements.at(i));
// 	fclose(outFile);
	
// 	outFile = fopen("top.ele","w");
// 	selNodes=theMesh.nodes.nselZ(0, theMesh.tol);
// 	selElements=theMesh.elements.getElementsWithNodes(selNodes,4);
// 	for( unsigned int i=0;i<selElements.size();i++)
// 		fprintf(outFile,"%d 0 0 0 0 0 0 0 0 0 0 0 0 0 0\n",selElements.at(i));
// 	fclose(outFile);
	
// 	outFile = fopen("inletClean.ele","w");
// 	selNodes=theMesh.nodes.nselY(theMesh.length*theMesh.reps,theMesh.length*theMesh.reps, theMesh.tol);
// 	selElements=theMesh.elements.getElementsWithNodes(selNodes,4);
// 	for( unsigned int i=0;i<selElements.size();i++)
// 		fprintf(outFile,"%d 0 0 0 0 0 0 0 0 0 0 0 0 0 0\n",selElements.at(i));
// 	fclose(outFile);
	
// 	outFile = fopen("outletClean.ele","w");
// 	selNodes=theMesh.nodes.nselY(0,0, theMesh.tol);
// 	selElements=theMesh.elements.getElementsWithNodes(selNodes,4);
// 	for( unsigned int i=0;i<selElements.size();i++)
// 		fprintf(outFile,"%d 0 0 0 0 0 0 0 0 0 0 0 0 0 0\n",selElements.at(i));
// 	fclose(outFile);
	
// 	outFile = fopen("outside.ele","w");
// 	selNodes=theMesh.nodes.nselX(-theMesh.width,-theMesh.width, theMesh.tol);
// 	selElements=theMesh.elements.getElementsWithNodes(selNodes,0);
// 	for( unsigned int i=0;i<selElements.size();i++)
// 		fprintf(outFile,"%d 0 0 0 0 0 0 0 0 0 0 0 0 0 0\n",selElements.at(i));
// 	fclose(outFile);
	
// 	outFile = fopen("allbutInside.nod","w");
// 	selNodes=theMesh.nodes.nselX(-100000000,theMesh.refPoints.at(0).x, theMesh.tol);
// 	for( unsigned int i=0;i<selNodes.size();i++)
// 		fprintf(outFile,"%d 0 0 0 0 0 0\n",selNodes.at(i));
// 	fclose(outFile);
	
	outFile = fopen("slices.inp","w");
	int N=theMesh.ldiv;
	for(int j=0;j<N;j++)
	{
		double length=theMesh.length*theMesh.reps*0.066000066;
		
		double ydiv=theMesh.ldiv;
		double y_low=((double)j+0)*length/ydiv;
		//double y_low_heat=((double)j-3)*length/ydiv;
		double y_low_heat=((double)j-1)*length/ydiv;
		double y_high=((double)j+1)*length/ydiv;
		char sliceName[50];
		sprintf(sliceName,"internal_slice%d",j);
		fprintf(outFile,"*ELSET,ELSET=%s\n",sliceName);
		
		//selNodes=theMesh.nodes.nselY(y_low,y_high,theMesh.tol);
		selNodes=theMesh.nodes.nselXYZ(-1-theMesh.tol*10,1+theMesh.tol*10,y_low-theMesh.tol,y_high+theMesh.tol,-1-theMesh.tol*10,1+theMesh.tol*10);
		selElements=theMesh.esln(selNodes,1);
		for( unsigned int i=0;i<selElements.size();i++)
		 	fprintf(outFile,"%d\n",selElements.at(i));
		fprintf(outFile,"*NSET,NSET=%s\n",sliceName);
		for( unsigned int i=0;i<selNodes.size();i++)
		 	fprintf(outFile,"%d\n",selNodes.at(i));

		sprintf(sliceName,"internal_heat_slice%d",j);
		fprintf(outFile,"*ELSET,ELSET=%s\n",sliceName);
		//selNodes=theMesh.nodes.nselY(y_low_heat,y_high,theMesh.tol);
		selNodes=theMesh.nodes.nselXYZ(-1-theMesh.tol*10,1+theMesh.tol*10,y_low_heat-theMesh.tol,y_high+theMesh.tol,-1-theMesh.tol*10,1+theMesh.tol*10);
		selElements=theMesh.esln(selNodes,1);
		for( unsigned int i=0;i<selElements.size();i++)
		 	fprintf(outFile,"%d\n",selElements.at(i));
		fprintf(outFile,"*NSET,NSET=%s\n",sliceName);
		for( unsigned int i=0;i<selNodes.size();i++)
		 	fprintf(outFile,"%d\n",selNodes.at(i));

		sprintf(sliceName,"external_slice%d",j);
		fprintf(outFile,"*ELSET,ELSET=%s\n",sliceName);
		//selNodes=theMesh.nodes.nselY(y_low,y_high,theMesh.tol);
		selNodes=theMesh.nodes.nselXYZam(-1+theMesh.tol,1-theMesh.tol,y_low-theMesh.tol,y_high+theMesh.tol,-1+theMesh.tol,1-theMesh.tol);
		selElements=theMesh.esln(selNodes,1);
		for( unsigned int i=0;i<selElements.size();i++)
		 	fprintf(outFile,"%d\n",selElements.at(i));
		fprintf(outFile,"*NSET,NSET=%s\n",sliceName);
		for( unsigned int i=0;i<selNodes.size();i++)
		 	fprintf(outFile,"%d\n",selNodes.at(i));

		sprintf(sliceName,"external_heat_slice%d",j);
		fprintf(outFile,"*ELSET,ELSET=%s\n",sliceName);
		//selNodes=theMesh.nodes.nselY(y_low_heat,y_high,theMesh.tol);
		selNodes=theMesh.nodes.nselXYZam(-1+theMesh.tol,1-theMesh.tol,y_low_heat-theMesh.tol,y_high+theMesh.tol,-1+theMesh.tol,1-theMesh.tol);
		selElements=theMesh.esln(selNodes,1);
		for( unsigned int i=0;i<selElements.size();i++)
		 	fprintf(outFile,"%d\n",selElements.at(i));
		fprintf(outFile,"*NSET,NSET=%s\n",sliceName);
		for( unsigned int i=0;i<selNodes.size();i++)
		 	fprintf(outFile,"%d\n",selNodes.at(i));

	}
	fclose(outFile);

	outFile = fopen("sets.inp","w");
	selNodes=theMesh.nodes.nselY(0,0,theMesh.tol);
	fprintf(outFile,"*NSET,NSET=botNodes\n");
	for( unsigned int i=0;i<selNodes.size();i++)
		 	fprintf(outFile,"%d\n",selNodes.at(i));
	
	selNodes=theMesh.nodes.nselY(theMesh.length*0.066000066,theMesh.length*0.066000066,theMesh.tol);
	fprintf(outFile,"*NSET,NSET=topNodes\n");
	for( unsigned int i=0;i<selNodes.size();i++)
		 	fprintf(outFile,"%d\n",selNodes.at(i));
	
	selNodes=theMesh.nodes.nselXYZ(-1.0-theMesh.tol*10,1.0+theMesh.tol*10,-1000000,100000,-1.0-theMesh.tol*10,1.0+theMesh.tol*10);
	selElements=theMesh.esln(selNodes,1);
	fprintf(outFile,"*NSET,NSET=internal\n");
	for( unsigned int i=0;i<selNodes.size();i++)
		 	fprintf(outFile,"%d\n",selNodes.at(i));
	fprintf(outFile,"*ELSET,ELSET=internal\n");
	for( unsigned int i=0;i<selElements.size();i++)
		 	fprintf(outFile,"%d\n",selElements.at(i));
	
	selNodes=theMesh.nodes.nselXYZ(-0.5-theMesh.tol*10,0.5+theMesh.tol*10,-1000000,100000,-0.5-theMesh.tol*10,0.5+theMesh.tol*10);
	selElements=theMesh.esln(selNodes,1);
	fprintf(outFile,"*NSET,NSET=core\n");
	for( unsigned int i=0;i<selNodes.size();i++)
		 	fprintf(outFile,"%d\n",selNodes.at(i));
	fprintf(outFile,"*ELSET,ELSET=core\n");
	for( unsigned int i=0;i<selElements.size();i++)
		 	fprintf(outFile,"%d\n",selElements.at(i));

	selNodes=theMesh.nodes.nselXYZinverted(-1.0,1.0,-1000000,100000,-1.0,1.0);
	selElements=theMesh.esln(selNodes,1);
	fprintf(outFile,"*NSET,NSET=external\n");
	for( unsigned int i=0;i<selNodes.size();i++)
		 	fprintf(outFile,"%d\n",selNodes.at(i));
	fprintf(outFile,"*ELSET,ELSET=external\n");
	for( unsigned int i=0;i<selElements.size();i++)
		 	fprintf(outFile,"%d\n",selElements.at(i));

	fclose(outFile);
	
	printf("Mesh written\n");
}

bool partOf(vector<int> A, vector<int> B) //is the domain A a part of domain B?
{
	for(int i=0; i<(int)A.size();i++)
	{
		bool foundElement=false;
		for(int j=0; j<(int)B.size();j++)
		{
			if(A.at(i)==B.at(j))
			{
				foundElement=true;
				break;
			}
		}
		if(foundElement==false)
		{
			return false;
		}
	}
	
	return true;
}

bool partOfSpecial(vector<int> A, vector<int> B, int number) //is the domain A a part of domain B (with "number" elements)?
{
	int counter = 0;
	for(int i=0; i<(int)A.size();i++)
	{
		bool foundElement=false;
		for(int j=0; j<(int)B.size();j++)
		{
			if(A.at(i)==B.at(j))
			{
				foundElement=true;
				counter++;
				break;
			}
		}
	}
	
	if(counter==number)
		return true;
	else
		return false;
}

bool intersect(vector<int> A, vector<int> B) //do domain A intersect domain B?
{
	for(int i=0; i<(int)A.size();i++)
	{
		for(int j=0; j<(int)B.size();j++)
		{
			if(A.at(i)==B.at(j))
			{
				return true;
			}
		}
	}
	
	
	return false;
}

vector<int> getIntersection(vector<int> A, vector<int> B) //returns all common elements of A and B
{
	vector<int> inter;
	
	for(int i=0; i<(int)A.size();i++)
	{
		for(int j=0; j<(int)B.size();j++)
		{
			if(A.at(i)==B.at(j))
				inter.push_back(A.at(i));
		}
	}
	
	return inter;
}

vector<int> subtract(vector<int> A, vector<int> B) //returns elements of A - B
{
	vector<int> diff;
	bool found;
	
	for(int i=0; i<(int)A.size();i++)
	{
		found=false;
		for(int j=0; j<(int)B.size();j++)
		{
			if(A.at(i)==B.at(j))
			{
				found=true;
				break;		
			}
		}
		if(not found)
			diff.push_back(A.at(i));
	}
	return diff;
}

vector<int> add(vector<int> A, vector<int> B) //returns elements of A + B
{
	vector<int> sum;
	sum.resize(A.size()+B.size());
	for(unsigned int i=0; i<sum.size();i++)
	{
		if(i<A.size())
			sum.at(i)=A.at(i);
		else
			sum.at(i)=B.at(i-A.size());
		//printf("node: %d \n",sum.at(i));
	}
	return sum;
}

