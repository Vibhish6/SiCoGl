# Graphics pipeline

For each object the renderer builds:

`MVP = Projection × View × Model`

The model matrix is `T × R(quaternion) × S`. The camera uses a perspective projection with configurable field of view and clipping planes. Selection uses the inverse of `Projection × View` to unproject a screen coordinate into a world-space ray, followed by a ray/axis-aligned bounding-box test.

The vertex shader transforms positions and normals. The fragment shader applies ambient, diffuse, and Blinn–Phong specular terms, with roughness and metallic parameters mapped to material response. Depth testing, back-face culling, alpha blending, indexed VAO/VBO/EBO rendering, a floor grid, and a cursor point are enabled in the OpenGL renderer.
