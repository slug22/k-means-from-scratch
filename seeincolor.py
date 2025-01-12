import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D

def plot_color_data(filename, title, subplot_pos):
    # Read data: x, y, color_sum, r, g, b
    data = np.loadtxt(filename)
    
    # Extract coordinates and colors
    x = data[:, 0]
    y = data[:, 1]
    z = data[:, 2]  # color sum
    colors = data[:, 3:6]  # rgb values
    
    ax = fig.add_subplot(subplot_pos, projection='3d')
    scatter = ax.scatter(x, y, z, c=colors, s=1)
    
    ax.set_xlabel('X (Image Width)')
    ax.set_ylabel('Y (Image Height)')
    ax.set_zlabel('Color Sum (R+G+B)')
    ax.set_title(title)
    
    # Adjust view angle for better visibility
    ax.view_init(elev=20, azim=45)

# Create figure
plt.figure(figsize=(15, 7))

# Plot original and segmented data side by side
fig = plt.figure(figsize=(15, 7))
plot_color_data('original_colors.txt', 'Original Image Colors', 121)
plot_color_data('segmented_colors.txt', 'Segmented Image Colors', 122)

plt.tight_layout()
plt.show()