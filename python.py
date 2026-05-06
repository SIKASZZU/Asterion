
import os
import math
from PIL import Image

def create_sprite_sheet(source_folder, output_name="sprite_sheet.png"):
    # Define the fixed size of your icons
    TILE_SIZE = 92
    
    # Filter for common image extensions
    valid_extensions = ('.png', '.jpg', '.jpeg', '.bmp', '.webp')
    images = [f for f in os.listdir(source_folder) if f.lower().endswith(valid_extensions)]
    
    if not images:
        print(f"No images found in {source_folder}")
        return

    # Sort images to keep the sprite order consistent
    images.sort()
    
    num_images = len(images)
    # Calculate grid dimensions (aiming for a square)
    columns = math.ceil(math.sqrt(num_images))
    rows = math.ceil(num_images / columns)

    # Create the blank canvas for the sprite sheet
    sprite_width = columns * TILE_SIZE
    sprite_height = rows * TILE_SIZE
    master_image = Image.new(mode='RGBA', size=(sprite_width, sprite_height), color=(0, 0, 0, 0))

    for index, image_name in enumerate(images):
        location = os.path.join(source_folder, image_name)
        with Image.open(location) as img:
            # Ensure the image is 92x92; resize if it isn't
            if img.size != (TILE_SIZE, TILE_SIZE):
                img = img.resize((TILE_SIZE, TILE_SIZE), Image.Resampling.LANCZOS)
            
            # Calculate (x, y) position in the grid
            x = (index % columns) * TILE_SIZE
            y = (index // columns) * TILE_SIZE
            
            # Paste the image onto the master sheet
            master_image.paste(img, (x, y))

    # Save the result
    master_image.save(output_name)
    print(f"Success! Created {output_name} ({columns}x{rows} grid).")


root_dir = 'assets/female/animations/Walking'
# root_dir = 'assets/female/animations'
# root_dir = 'assets/female/animations'

for folder in os.listdir(root_dir):
    folder_path = os.path.join(root_dir, folder)
    if os.path.isdir(folder_path):
        create_sprite_sheet(folder_path, f"{folder}.png")


# # Usage
# # Change 'my_pictures_folder' to your actual folder path
# create_sprite_sheet('path/to/your/folder', 'final_sprite.png')