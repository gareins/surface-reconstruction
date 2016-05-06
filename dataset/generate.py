import numpy as np
import gzip
import urllib.request
import os

print("Download from http://www.prinmath.com/csci5229/OBJ/index.html")
N = [100, 1000, 10000]

for npoints in N:
    vec = np.random.randn(3, npoints)
    vec /= np.linalg.norm(vec, axis=0)
    vec *= 5

    with open("sphere_" + str(npoints) + ".obj", "w") as fp:
        for pt in vec.T:
            fp.write("v " + ", ".join([str(p) for p in pt]) + "\n")
