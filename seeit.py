import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D

# Read the cluster data
data = np.loadtxt('cluster_data.txt')
X = data[:, 0:3]  # Points coordinates
labels = data[:, 3]  # Cluster labels

# Read the centroids
centroids = np.loadtxt('centroids.txt')

# Create 3D scatter plot
fig = plt.figure(figsize=(10, 8))
ax = fig.add_subplot(111, projection='3d')

# Plot points
colors = ['r', 'g', 'b', 'c', 'm', 'y']  # Add more colors if k > 6
for i in range(len(np.unique(labels))):
    cluster_points = X[labels == i]
    ax.scatter(cluster_points[:, 0], cluster_points[:, 1], cluster_points[:, 2], 
              c=colors[i], label=f'Cluster {i}', alpha=0.6, s=20)

# Plot centroids
ax.scatter(centroids[:, 0], centroids[:, 1], centroids[:, 2], 
          color='black', marker='*', s=200, label='Centroids')

ax.set_xlabel('X')
ax.set_ylabel('Y')
ax.set_zlabel('Z')
ax.set_title('K-means Clustering Results')
plt.legend()
plt.show()