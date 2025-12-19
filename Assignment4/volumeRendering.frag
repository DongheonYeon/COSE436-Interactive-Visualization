#version 140
#extension GL_ARB_compatibility: enable

in vec3 pixelPosition;
uniform vec3 eyePosition;
uniform vec3 objectMin;
uniform vec3 objectMax;
uniform vec3 up;
uniform sampler3D tex;
uniform float isoValue;


void main()
{
	vec4 composedColor=vec4(0,0,0,0);

	// Ray-cube intersection (2.1)
	vec3 rayDir = normalize(pixelPosition - eyePosition);
	vec3 invDir = 1.0 / rayDir;

	vec3 t0s = (objectMin - eyePosition) * invDir;
	vec3 t1s = (objectMax - eyePosition) * invDir;
	vec3 tsmaller = min(t0s, t1s);
	vec3 tbigger = max(t0s, t1s);

	float tmin = max(max(tsmaller.x, tsmaller.y), tsmaller.z);
	float tmax = min(min(tbigger.x, tbigger.y), tbigger.z);

	if (tmax < max(tmin, 0.0)) {
		gl_FragColor = composedColor;
		return;
	}

	vec3 entryPoint = eyePosition + rayDir * tmin;
	vec3 exitPoint = eyePosition + rayDir * tmax;

	// Ray marching + iso-surface detection (2.2)
	const int maxSteps = 1024;
	float tStart = max(tmin, 0.0);
	float tEnd = tmax;
	float dt = (tEnd - tStart) / float(maxSteps);
	dt = max(dt, 0.0005);

	vec3 boxSize = objectMax - objectMin;
	float prevT = tStart;
	vec3 prevPos = eyePosition + rayDir * prevT;
	vec3 prevTex = (prevPos - objectMin) / boxSize;
	float prevVal = texture(tex, clamp(prevTex, 0.0, 1.0)).r - isoValue;

	bool hit = false;
	float hitT = tEnd;

	for (int i = 0; i < maxSteps; ++i) {
		float t = tStart + dt * float(i + 1);
		if (t > tEnd) break;
		vec3 pos = eyePosition + rayDir * t;
		vec3 texPos = (pos - objectMin) / boxSize;
		float val = texture(tex, clamp(texPos, 0.0, 1.0)).r - isoValue;

		if (prevVal * val <= 0.0) {
			// Binary search refinement for zero-crossing
			float a = prevT;
			float b = t;
			float fa = prevVal;
			for (int it = 0; it < 8; ++it) {
				float m = 0.5 * (a + b);
				vec3 mpos = eyePosition + rayDir * m;
				vec3 mtex = (mpos - objectMin) / boxSize;
				float fm = texture(tex, clamp(mtex, 0.0, 1.0)).r - isoValue;
				if (fa * fm <= 0.0) {
					b = m;
					val = fm;
				} else {
					a = m;
					fa = fm;
				}
			}
			hitT = b;
			hit = true;
			break;
		}

		prevT = t;
		prevVal = val;
	}

	if (!hit) {
		gl_FragColor = vec4(1.0, 1.0, 1.0, 1.0);
		return;
	}

	vec3 hitPos = eyePosition + rayDir * hitT;
	vec3 hitTex = (hitPos - objectMin) / boxSize;
	hitTex = clamp(hitTex, 0.0, 1.0);

	// Gradient-based normal in texture space
	vec3 delta = vec3(1.0 / 256.0);
	float gx = texture(tex, clamp(hitTex + vec3(delta.x, 0.0, 0.0), 0.0, 1.0)).r -
	           texture(tex, clamp(hitTex - vec3(delta.x, 0.0, 0.0), 0.0, 1.0)).r;
	float gy = texture(tex, clamp(hitTex + vec3(0.0, delta.y, 0.0), 0.0, 1.0)).r -
	           texture(tex, clamp(hitTex - vec3(0.0, delta.y, 0.0), 0.0, 1.0)).r;
	float gz = texture(tex, clamp(hitTex + vec3(0.0, 0.0, delta.z), 0.0, 1.0)).r -
	           texture(tex, clamp(hitTex - vec3(0.0, 0.0, delta.z), 0.0, 1.0)).r;
	vec3 N = normalize(vec3(gx, gy, gz));

	// Phong shading
	vec3 L = normalize(vec3(1.0, 1.0, 1.0));
	vec3 V = normalize(eyePosition - hitPos);
	vec3 R = reflect(-L, N);
	float diff = max(dot(N, L), 0.0);
	float spec = pow(max(dot(R, V), 0.0), 32.0);
	vec3 ambient = vec3(0.1);
	vec3 color = ambient + diff * vec3(0.9) + spec * vec3(0.6);

	composedColor = vec4(color, 1.0);

	
    gl_FragColor = composedColor;
}
