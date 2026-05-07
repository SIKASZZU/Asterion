
import os
import math
from PIL import Image

def create_sprite_sheet(source_folder, output_name="sprite_sheet.png"):
    # Define the fixed size of your icons
    TILE_SIZE = 144
    
    # Filter for common image extensions
    valid_extensions = ('.png', '.jpg', '.jpeg', '.bmp', '.webp')
    images = [f for f in os.listdir(source_folder) if f.lower().endswith(valid_extensions)]
    
    if not images:
        print(f"No images found in {source_folder}")
        return

    # Sort images to keep the sprite order consistent
    images.sort()
    
    num_images = len(images)
    
    # Force a single row layout
    columns = num_images
    rows = 1

    # Create the blank canvas for the sprite sheet (Width will be TILE_SIZE * total images)
    sprite_width = columns * TILE_SIZE
    sprite_height = TILE_SIZE # Only one tile high
    master_image = Image.new(mode='RGBA', size=(sprite_width, sprite_height), color=(0, 0, 0, 0))

    for index, image_name in enumerate(images):
        location = os.path.join(source_folder, image_name)
        with Image.open(location) as img:
            # Resize if necessary
            if img.size != (TILE_SIZE, TILE_SIZE):
                img = img.resize((TILE_SIZE, TILE_SIZE), Image.Resampling.LANCZOS)
            
            # Calculate x position (y is always 0 for a single row)
            x = index * TILE_SIZE
            y = 0
            
            # Paste the image onto the master sheet
            master_image.paste(img, (x, y))

    # Save the result
    master_image.save(output_name)
    print(f"Success! Created {output_name} (Single row of {num_images} images).")

# root_dir = 'assets/female/animations/Walking'
root_dir = 'walls'

# Check if the 'walls' directory actually exists
if os.path.exists(root_dir):
    create_sprite_sheet(root_dir, "walls_sprite_sheet.png")
    for folder in os.listdir(root_dir):
        folder_path = os.path.join(root_dir, folder)
        
        # Only process actual folders inside 'walls'
        if os.path.isdir(folder_path):
            # This saves 'stone_wall.png' in your main directory
            create_sprite_sheet(folder_path, f"{folder}.png")
else:
    print(f"Error: The folder '{root_dir}' was not found.")


# # Usage
# # Change 'my_pictures_folder' to your actual folder path
# create_sprite_sheet('path/to/your/folder', 'final_sprite.png')