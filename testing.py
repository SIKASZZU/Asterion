import pygame
import sys

# Initialize Pygame
pygame.init()

# Load and prepare image (after dummy display)
pygame.display.set_mode((100, 100))  # Needed for convert_alpha
# sprite_sheet = pygame.image.load("animatsion.png").convert_alpha()
sprite_sheet = pygame.image.load("run_spritesheet.png").convert_alpha()

# Original frame and animation settings
ORIG_FRAME_WIDTH = 256
ORIG_FRAME_HEIGHT = sprite_sheet.get_height()
NUM_FRAMES = 16

# Desired scaling
SCALE = 1
FRAME_WIDTH = ORIG_FRAME_WIDTH * SCALE
FRAME_HEIGHT = ORIG_FRAME_HEIGHT * SCALE

# Set actual screen
screen = pygame.display.set_mode((FRAME_WIDTH, FRAME_HEIGHT))
pygame.display.set_caption("Enlarged Sprite Animation")

# Extract and scale frames
frames = []
for i in range(NUM_FRAMES):
    frame_rect = pygame.Rect(i * ORIG_FRAME_WIDTH, 0, ORIG_FRAME_WIDTH, ORIG_FRAME_HEIGHT)
    frame = sprite_sheet.subsurface(frame_rect)
    scaled_frame = pygame.transform.scale(frame, (FRAME_WIDTH, FRAME_HEIGHT))
    frames.append(scaled_frame)

# Animation loop settings
clock = pygame.time.Clock()
FPS = 25  # 1 frame per second (i.e., 1s delay)
current_frame = 0
running = True

# Main loop
while running:
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False

    screen.fill((0, 0, 0))  # Clear screen
    screen.blit(frames[current_frame], (0, 0))
    pygame.display.flip()

    current_frame = (current_frame + 1) % NUM_FRAMES
    clock.tick(FPS)

pygame.quit()
sys.exit()

