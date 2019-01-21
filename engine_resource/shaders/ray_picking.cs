    // ����������� ������
	// ������
	struct Point
    {
        float3 pos;
    };

    struct Word
    {
        int index;
    };
	
	struct Transformation
    {
        matrix pos;
		matrix rot;
		matrix sca;
    };
	
	struct Offset // ��������
	{
		uint countVertexAllPred;
		uint countVertex;
		uint countIndecedsAllPred;
		uint countIndeceds;
		uint countTransformations;
	};

    StructuredBuffer<Point> arrayPoints: register(t0);
    StructuredBuffer<Word> arrayIndeceds: register(t1);
	StructuredBuffer<Transformation> arrayTransformations: register(t2);
	StructuredBuffer<Offset> arrayOffsets: register(t3);
	StructuredBuffer<Point> arrayBBoxPoints: register(t4);
	
	// ������
	struct DataOutput
	{
		int indexModel;
		int indexTransformation;
	};
	
	RWStructuredBuffer<DataOutput> outputBuffer: register(u0);
	
	// ����������� ������
	cbuffer CommonData: register(b0)
    { 
        float4 cameraPosition; // ������� ������
		float4 rayPosition; // ������� ����
		float4 rayDirection; // ����������� ����
		float4 countModel; // ���������� �������
    };
	
	// ���� ������
	interface TriangleMethods
	{
		void createEmpty(); // ��������������� ������������ ������������
		bool isEmpty(); // �������� �� ����������� �����������
		void newTriangle(float3 x, float3 y, float3 z); // �������� ������������ + ������� ��� ������
		float3 getCenter(); // ��������� ������ ������������
	};
	
	class Triangle: TriangleMethods
	{
		float3 a; // ������� ������������
		float3 b;
		float3 c;
		float3 o; // ����� ������������	
		
		void createEmpty()
		{
			a = b = c = o = 0;
		}
		
		bool isEmpty()
		{
			float _a = a.x + a.y + a.z;
			float _b = b.x + b.y + b.z;
			float _c = c.x + c.y + c.z;
			return (_a == 0 && _b == 0 && _c == 0);
		}
		
		void newTriangle(float3 x, float3 y, float3 z)
		{
			a = x; b = y; c = z;
			float _x = (a.x + b.x + c.x) / 3;
			float _y = (a.y + b.y + c.y) / 3;
			float _z = (a.z + b.z + c.z) / 3;
			o = float3( _x, _y, _z );
		}
		
		float3 getCenter()
		{
			return o;
		}
	};
	
	// �������������� ������� ��� ��������
	// ��������� ���������� ������������� �� ������
	bool equalsSFromCamera(Triangle t1, Triangle t2) // t1 - current triangle, t2 - new triangle; true false - if t2 > t1, then return true
	{
		bool result = true;
		float length1 = length(t1.getCenter() - (float3)cameraPosition); // �������� ����� �� ������ �� ������� ������������
		float length2 = length(t2.getCenter() - (float3)cameraPosition); // �������� ����� �� ������ �� ������� ������������
		if(length1 < length2)
			result = false;
		return result;
	}
	
	// ���������� ������� ��������
	float3 generateNormal(float3 a, float3 b, float3 c) 
	{
		float3 u = b - a;
		float3 v = c - a;
		float3 p = normalize(cross(u, v));
		return p;
	}
	
	// ����� �� ��� � ���������
	float intersectionRayAndPlane(float3 p, float3 normal)
	{
		return -dot((float3)rayPosition - p, normal) / dot((float3)rayDirection, normal);
	}
	
	// ����� �� ����� �� ������������
	int belongsPointInTriangle(float3 a, float3 b, float3 c, float3 m, float3 n)
	{	
		int result = 0;
		float3 tmp;
		// ������ ��� �������
		float3 oa = a - m;
		float3 ob = b - m;
		tmp = dot(n, cross(oa, ob));
		float v1 = tmp.x;
	
		// ������ ��� �������
		float3 oc = c - m;
		tmp = dot(n, cross(ob, oc));
		float v2 = tmp.x;

		// ������ ��� �������
		tmp = dot(n, cross(oc, oa));
		float v3 = tmp.x;

		// ������ ��������� �����
		if ((v1 < 0 && v2 < 0 && v3 < 0) || (v1 >= 0 && v2 >= 0 && v3 >= 0))
			result = 1;
		return result;
	}
	
	// ���������� �����������, ������� ����� � ������
	Triangle getNearTriangleFromCamera(Triangle t1, Triangle t2)
	{
		Triangle t = t2;
		float length1 = length(t1.getCenter() - (float3)cameraPosition); // �������� ����� �� ������ �� ������� ������������
		float length2 = length(t2.getCenter() - (float3)cameraPosition); // �������� ����� �� ������ �� ������� ������������
		if(length1 < length2)
			t = t1;
		return t;
	}
	
	// ������� ���������� ����� ����������� ���� � ���������
	float3 rayIsTriangle(inout  float3 p0, inout  float3 p1, inout  float3 p2, inout  float3 n, int off_matrix)
	{
		matrix _pos = arrayTransformations[off_matrix].pos;
		matrix _rot = arrayTransformations[off_matrix].rot;
		matrix _sca = arrayTransformations[off_matrix].sca;
		
		p0 = (float3)mul(float4(p0, 1), _sca);
		p0 = (float3)mul(float4(p0, 1), _rot);
		p0 = (float3)mul(float4(p0, 1), _pos);

		p1 = (float3)mul(float4(p1, 1), _sca);
		p1 = (float3)mul(float4(p1, 1), _rot);
		p1 = (float3)mul(float4(p1, 1), _pos);
					
		p2 = (float3)mul(float4(p2, 1), _sca);
		p2 = (float3)mul(float4(p2, 1), _rot);
		p2 = (float3)mul(float4(p2, 1), _pos);
		// ���������� ������� ��������
		n = generateNormal(p0, p1, p2);
		// ���������, ����� �� �������(�� ������, ����� ��) // ???
		// ���������, ���������� �� ��� ���������
		float t = intersectionRayAndPlane(p0, n);
		// �������� �������� �� ��, ��� ��� ����� � ������� // ???
		// �������� ����� ����������� ���� � ��������
		float3 m = (float3)rayPosition + (float3)mul(t, (float3)rayDirection);
		return m;
	}
	
	
	// ��������� �������
	[numthreads(1, 1, 1)]
	void main(uint3 groupThreadID : SV_GroupThreadID)
	{
		Triangle tri; // ����� ������� � ������ �����������
		tri.createEmpty(); // ��������� ����������� �����������
		DataOutput dataTriangle; // ������ ������������
		dataTriangle.indexModel = -1;
		dataTriangle.indexTransformation = -1;
		int indexModel = -1; // ������� ������ ������
		int indexModelMatrix = -1; // ������� ������ ������� ������
		float3 n = (float3)0; // ���������� �������
		int rayInModel = 0; // bbox - ����� �� ��� � ����(> 0 == �����)
		int offTrans = 0; // ��������, ��� ������� � ��������
		int offVertex = 0; // ��������, ��� ����������� ������� � ��������
		
		//[unroll]
		for(int k = 0; k <countModel.x;k++) 
		{
			indexModel++;
			int start = arrayOffsets[k].countIndecedsAllPred;
			int end = start + arrayOffsets[k].countIndeceds;
			int countTransf = (int)arrayOffsets[k].countTransformations;
			for(int j = 0;j<countTransf;j++) // ���������� ��� �������
			{
				indexModelMatrix++;
				for(int l = 0; l < 36; l+=3) // ������� ��������� bbox ���� �������
				{
					int ind = k * 36; // ��������� �������
					float3 a = arrayBBoxPoints[ind + l].pos; float3 b = arrayBBoxPoints[ind + l + 1].pos; float3 c = arrayBBoxPoints[ind + l + 2].pos;
					float3 m = rayIsTriangle(a, b, c, n, offTrans + j);
					rayInModel += belongsPointInTriangle(a, b, c, m, n);
				}
				if(rayInModel == 0)
					continue;
				
				
			for(int i = start;i<end;i+=3) // ���������� ��� �������
			{
				// �������� ��� ������� - �������
				int i1 = (int)arrayIndeceds[i].index;
				int i2 = (int)arrayIndeceds[i+1].index;
				int i3 = (int)arrayIndeceds[i+2].index;
				// �������� ������� ��������
				float3 p0 = arrayPoints[offVertex + i1].pos;
				float3 p1 = arrayPoints[offVertex + i2].pos;
				float3 p2 = arrayPoints[offVertex + i3].pos;

					float3 m = rayIsTriangle(p0, p1, p2, n, offTrans + j);
					// ���������, ����� �� ����� �� ������������, � ���� �� - �� ���������� ������� ����������� � ����� �� ��������� �� ������
					if(belongsPointInTriangle(p0, p1, p2, m, n) == 1)
					{
						Triangle triangleNew;
						triangleNew.newTriangle(p0, p1, p2);
						triangleNew = getNearTriangleFromCamera(tri, triangleNew); // �������� ����������� �� ����� ������� � ������
						// ���������� ������������
						if(tri.isEmpty() || !equalsSFromCamera(tri, triangleNew)) // ���� ����������� ��������� ��� ��� ������ ��������
						{
							// �������� ������� ������� �� cpu
							dataTriangle.indexModel = indexModel;
							dataTriangle.indexTransformation = indexModelMatrix;
							tri = triangleNew;
						}
					}
			}
			rayInModel = 0;
			}
			offVertex += arrayOffsets[k].countVertex;
			indexModelMatrix = -1;
			offTrans += (int)arrayOffsets[k].countTransformations;
		}
		// ���������� ���������� ��������� � �������� ����� (���� ������� == -1, ������ ��� ������ �� �����)
		outputBuffer[0].indexModel = dataTriangle.indexModel;
		outputBuffer[0].indexTransformation = dataTriangle.indexTransformation;
		
	}
	
	
	