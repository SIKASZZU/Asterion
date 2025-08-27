import pygame
import sys

# Initialize Pygame
pygame.init()

# Load and prepare image (after dummy display)
pygame.display.set_mode((1, 1))  # Needed for convert_alpha
sprite_sheet = pygame.image.load("resources/player_animation.png").convert_alpha()

# Original frame and animation settings
ORIG_FRAME_WIDTH = 32
ORIG_FRAME_HEIGHT = sprite_sheet.get_height()
NUM_FRAMES = 4

# Desired scaling
SCALE = 4
FRAME_WIDTH = ORIG_FRAME_WIDTH * SCALE
FRAME_HEIGHT = ORIG_FRAME_HEIGHT * SCALE

# Set actual screen
screen = pygame.display.set_mode((FRAME_WIDTH*2, FRAME_HEIGHT))
pygame.display.set_caption("Sprite Animation")

# Extract and scale frames
frames = []
for i in range(NUM_FRAMES):
    frame_rect = pygame.Rect(i * ORIG_FRAME_WIDTH, 0, ORIG_FRAME_WIDTH, ORIG_FRAME_HEIGHT)
    frame = sprite_sheet.subsurface(frame_rect)
    scaled_frame = pygame.transform.scale(frame, (FRAME_WIDTH, FRAME_HEIGHT))
    frames.append(scaled_frame)

# Animation loop settings
clock = pygame.time.Clock()
FPS = 4  # 1 frame per second (i.e., 1s delay)
current_frame = 0
idle_frame_index = 4

frame_rect = pygame.Rect(idle_frame_index*ORIG_FRAME_WIDTH, 0, ORIG_FRAME_WIDTH, ORIG_FRAME_HEIGHT)
frame = sprite_sheet.subsurface(frame_rect)
scaled_frame = pygame.transform.scale(frame, (FRAME_WIDTH, FRAME_HEIGHT))
frames.append(scaled_frame)

running = True

# Main loop
while running:
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False

    screen.fill((0, 0, 0))  # Clear screen
    screen.blit(frames[current_frame], (0, 0))
    screen.blit(frames[idle_frame_index], (FRAME_WIDTH, 0))
    pygame.display.flip()

    current_frame = (current_frame + 1) % NUM_FRAMES
    clock.tick(FPS)

pygame.quit()
sys.exit()

