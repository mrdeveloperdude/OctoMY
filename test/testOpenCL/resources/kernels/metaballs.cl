#pragma OPENCL EXTENSION cl_amd_printf : enable
#pragma OPENCL EXTENSION cl_intel_printf : enable

kernel void metaballs(global const int2 *sources, int num, float charge, float limit, int apply, __global uint *pbo){

/*
	int x=get_global_id(0);
	int y=get_global_id(1);
	int gid= x+w*PARAM_SCREEN_WIDTH;
	float sum=0.0f;
	int i=0;
	for(i=0;i<num;i++){
		float dx=(x-sources[i].x)/charge;
		float dy=(y-sources[i].y)/charge;
		float val=exp(-dx*dx-dy*dy);
		sum+=val;
	}
	if(apply){
		sum=sum>limit?1.0f:0.0f;
	}
	uint r = sum*256.0;
	uint g = sum*256.0;
	uint b = sum*256.0;
	uint a=0;
	pbo[gid] = r | (g << 8) | (b << 16)| (a << 24);
	*/

}

