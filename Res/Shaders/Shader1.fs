#version 440
in vec2 fragUvs;
in vec3 fragNormal;
in vec4 fragPos;

out vec4 finalColor;

void main () {
	vec3 materialColor = min(vec3(0.5, 0.5, 0.5) + vec3(fragUvs.x, fragUvs.y, 0.0f), vec3(1.0, 1.0, 1.0));

	float diffuseFactor = max(dot(normalize(fragNormal), normalize(vec3(0.0, 8.0, 8)-fragPos.xyz)), 0.0);
	vec3 color = vec3(0.2, 0.2, 0.2)*materialColor + diffuseFactor*materialColor;

	// Normal color
	color = (normalize(fragNormal)+1.0)/2.0;

	// Position color
	//color = ((fragPos.xyz+10.0)/20.0) + min(0.0, dot(normalize(fragNormal), normalize(fragNormal)));

	if(min(fragUvs.x, 0.0) == 0)
	finalColor = vec4(min(color, vec3(1.0, 1.0, 1.0)), 1.0);
}