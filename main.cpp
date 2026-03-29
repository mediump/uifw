#include "UI/Core/Application.hpp"
#include "UI/ECS/Components/BaseComponent.hpp"
#include "UI/ECS/Components/RenderingComponents.hpp"
#include "UI/GFX/Renderer/Text/TextHelpers.hpp"
#include "UI/GFX/Shader.hpp"
#include "UI/IO/Text/FontLoader.hpp"
#include "UI/Window/Window.hpp"

#include <chrono>

#include "UI/ECS/Components/InputComponents.hpp"
#include "Utils.hpp"

#define DEMO_WINDOW_WIDTH  1280
#define DEMO_WINDOW_HEIGHT 720

#define DPI_SCALE          1.25

static ui::Color4f lerpColor(const ui::Color4f &a, const ui::Color4f &b, const float t)
{
  return {a.r + (b.r - a.r) * t, a.g + (b.g - a.g) * t, a.b + (b.b - a.b) * t,
          a.a + (b.a - a.a) * t};
}

int main()
{
  ui::ApplicationData app = ui::Application::init();

  ui::WindowData *window =
    ui::Window::initializeWindow("Buttons", DEMO_WINDOW_WIDTH, DEMO_WINDOW_HEIGHT, &app);
  ui::WindowData *window2 =
    ui::Window::initializeWindow("Text/Scroll Area", 512, 512, &app);

  ui::FontData fontData = ui::FontLoader::loadFont("res/fonts/_generated/Roboto.png",
                                                   "res/fonts/_generated/Roboto.json");

  /* ---------------------------- Setup scene ---------------------------- */
  auto layoutComponent = window->canvas.entity.get_ref<ui::LayoutComponent>();

  layoutComponent->type = ui::LayoutType_Horizontal;
  layoutComponent->margins = {10, 10, 10, 10};
  layoutComponent->spacing = 10;

  const auto e1 = ui::ecs::createEntity(&window->ecsRoot, 0, 0, 50, 50, "Entity1",
                                        &window->canvas.entity);

  auto e1Base = e1.get_ref<ui::ecs::BaseComponent>();

  e1Base->minWidth = 500;
  e1Base->maxWidth = 500;

  e1.set<ui::LayoutComponent>({
    .type = ui::LayoutType_Vertical,
    .margins = {10, 10, 10, 10},
    .spacing = 0,
  });

  const auto framerateEntity = ui::TextHelpers::createTextEntity(
    &window->ecsRoot, &fontData, "Framerate: ", {1.0, 1.0, 0.0, 1.0}, 16, 3, 3, 250, 50,
    "FramerateText");

  size_t currentY = 36;

  constexpr ui::Color4f gradientStart = {1.0f, 1.0f, 1.0f, 1.0f};
  constexpr ui::Color4f gradientEnd = {0.5f, 0.5f, 0.5f, 1.0f};

  // for (uint32_t i = 0; i < 20; ++i) {
  //   const std::string name = std::string("TextEntity") + std::to_string(i);
  //   const float t = static_cast<float>(i) / 19.0f;
  //   const ui::Color4f color = lerpColor(gradientStart, gradientEnd, t);
  //
  //   ui::TextHelpers::createTextEntity(&window.ecsRoot, &fontData,
  //                                     "Pack my box with five-dozen liquor jugs.",
  //                                     color, 20, 16, currentY, 128, 128, name.c_str());
  //
  //   currentY += 26 * static_cast<uint32_t>(fontData.metrics.lineHeight); // Font size =
  //   20
  // }

  const auto e2 = ui::ecs::createEntity(&window2->ecsRoot, 0, 0, 50, 50, "Entity2",
                                        &window2->canvas.entity);
  e2.set<ui::LayoutComponent>(
    {.type = ui::LayoutType_Vertical, .margins = {10, 10, 10, 10}, .spacing = 3});

  /* ---- TEXT DISPLAY ---- */
  // const auto textDisplayEntity = ui::TextHelpers::createTextEntity(
  //   &window2->ecsRoot, &fontData,
  //   "Pack my box with five-dozen liquor jugs.\nPack my box with five-dozen liquor "
  //   "jugs.\nPack my box with five-dozen liquor jugs.\nPack my box with five-dozen
  //   liquor " "jugs.\nPack my box with five-dozen liquor jugs.\nPack my box with
  //   five-dozen liquor " "jugs.\nPack my box with five-dozen liquor jugs.\nPack my box
  //   with five-dozen liquor " "jugs.\nPack my box with five-dozen liquor jugs.\nPack my
  //   box with five-dozen liquor " "jugs.\nPack my box with five-dozen liquor jugs.\nPack
  //   my box with five-dozen liquor " "jugs.", {1.0f, 1.0f, 1.0f, 1.0f}, 32, 16,
  //   currentY, 128, 128, "BigText", &e2);

  const auto textDisplayEntity = ui::TextHelpers::createTextEntity(
    &window2->ecsRoot, &fontData,
    R"(SCENE 2

Another part of the island

Enter CALIBAN, STEPHANO, and TRINCULO

  STEPHANO. Tell not me-when the butt is out we will drink
    water, not a drop before; therefore bear up, and board
    'em. Servant-monster, drink to me.
  TRINCULO. Servant-monster! The folly of this island! They
    say there's but five upon this isle: we are three of
    them; if th' other two be brain'd like us, the state
    totters.
  STEPHANO. Drink, servant-monster, when I bid thee; thy
    eyes are almost set in thy head.
  TRINCULO. Where should they be set else? He were a brave
    monster indeed, if they were set in his tail.
  STEPHANO. My man-monster hath drown'd his tongue in
    sack. For my part, the sea cannot drown me; I swam, ere
    I could recover the shore, five and thirty leagues, off
    and on. By this light, thou shalt be my lieutenant,
    monster, or my standard.
  TRINCULO. Your lieutenant, if you list; he's no standard.  
  STEPHANO. We'll not run, Monsieur Monster.
  TRINCULO. Nor go neither; but you'll lie like dogs, and
    yet say nothing neither.
  STEPHANO. Moon-calf, speak once in thy life, if thou beest
    a good moon-calf.
  CALIBAN. How does thy honour? Let me lick thy shoe.
    I'll not serve him; he is not valiant.
  TRINCULO. Thou liest, most ignorant monster: I am in case
    to justle a constable. Why, thou debosh'd fish, thou,
    was there ever man a coward that hath drunk so much sack
    as I to-day? Wilt thou tell a monstrous lie, being but
    half fish and half a monster?
  CALIBAN. Lo, how he mocks me! Wilt thou let him, my
    lord?
  TRINCULO. 'Lord' quoth he! That a monster should be such
    a natural!
  CALIBAN. Lo, lo again! Bite him to death, I prithee.
  STEPHANO. Trinculo, keep a good tongue in your head; if
    you prove a mutineer-the next tree! The poor monster's
    my subject, and he shall not suffer indignity.  
  CALIBAN. I thank my noble lord. Wilt thou be pleas'd to
    hearken once again to the suit I made to thee?
  STEPHANO. Marry will I; kneel and repeat it; I will stand,
    and so shall Trinculo.

                     Enter ARIEL, invisible

  CALIBAN. As I told thee before, I am subject to a tyrant,
    sorcerer, that by his cunning hath cheated me of the
    island.
  ARIEL. Thou liest.
  CALIBAN. Thou liest, thou jesting monkey, thou;
    I would my valiant master would destroy thee.
    I do not lie.
  STEPHANO. Trinculo, if you trouble him any more in's tale,
    by this hand, I will supplant some of your teeth.
  TRINCULO. Why, I said nothing.
  STEPHANO. Mum, then, and no more. Proceed.
  CALIBAN. I say, by sorcery he got this isle;
    From me he got it. If thy greatness will  
    Revenge it on him-for I know thou dar'st,
    But this thing dare not-
  STEPHANO. That's most certain.
  CALIBAN. Thou shalt be lord of it, and I'll serve thee.
  STEPHANO. How now shall this be compass'd? Canst thou
    bring me to the party?
  CALIBAN. Yea, yea, my lord; I'll yield him thee asleep,
    Where thou mayst knock a nail into his head.
  ARIEL. Thou liest; thou canst not.
  CALIBAN. What a pied ninny's this! Thou scurvy patch!
    I do beseech thy greatness, give him blows,
    And take his bottle from him. When that's gone
    He shall drink nought but brine; for I'll not show him
    Where the quick freshes are.
  STEPHANO. Trinculo, run into no further danger; interrupt
    the monster one word further and, by this hand, I'll turn
    my mercy out o' doors, and make a stock-fish of thee.
  TRINCULO. Why, what did I? I did nothing. I'll go farther
    off.
  STEPHANO. Didst thou not say he lied?  
  ARIEL. Thou liest.
  STEPHANO. Do I so? Take thou that.  [Beats him]  As you like
    this, give me the lie another time.
  TRINCULO. I did not give the lie. Out o' your wits and
    hearing too? A pox o' your bottle! This can sack and
    drinking do. A murrain on your monster, and the devil
    take your fingers!
  CALIBAN. Ha, ha, ha!
  STEPHANO. Now, forward with your tale.-Prithee stand
    further off.
  CALIBAN. Beat him enough; after a little time, I'll beat
    him too.
  STEPHANO. Stand farther. Come, proceed.
  CALIBAN. Why, as I told thee, 'tis a custom with him
    I' th' afternoon to sleep; there thou mayst brain him,
    Having first seiz'd his books; or with a log
    Batter his skull, or paunch him with a stake,
    Or cut his wezand with thy knife. Remember
    First to possess his books; for without them
    He's but a sot, as I am, nor hath not  
    One spirit to command; they all do hate him
    As rootedly as I. Burn but his books.
    He has brave utensils-for so he calls them-
    Which, when he has a house, he'll deck withal.
    And that most deeply to consider is
    The beauty of his daughter; he himself
    Calls her a nonpareil. I never saw a woman
    But only Sycorax my dam and she;
    But she as far surpasseth Sycorax
    As great'st does least.
  STEPHANO. Is it so brave a lass?
  CALIBAN. Ay, lord; she will become thy bed, I warrant,
    And bring thee forth brave brood.
  STEPHANO. Monster, I will kill this man; his daughter and I
    will be King and Queen-save our Graces!-and Trinculo
    and thyself shall be viceroys. Dost thou like the plot,
    Trinculo?
  TRINCULO. Excellent.
  STEPHANO. Give me thy hand; I am sorry I beat thee; but
    while thou liv'st, keep a good tongue in thy head.  
  CALIBAN. Within this half hour will he be asleep.
    Wilt thou destroy him then?
  STEPHANO. Ay, on mine honour.
  ARIEL. This will I tell my master.
  CALIBAN. Thou mak'st me merry; I am full of pleasure.
    Let us be jocund; will you troll the catch
    You taught me but while-ere?
  STEPHANO. At thy request, monster, I will do reason, any
    reason. Come on, Trinculo, let us sing.              [Sings]

    Flout 'em and scout 'em,
    And scout 'em and flout 'em;
    Thought is free.

  CALIBAN. That's not the tune.
                      [ARIEL plays the tune on a tabor and pipe]
  STEPHANO. What is this same?
  TRINCULO. This is the tune of our catch, play'd by the
    picture of Nobody.
  STEPHANO. If thou beest a man, show thyself in thy  
    likeness; if thou beest a devil, take't as thou list.
  TRINCULO. O, forgive me my sins!
  STEPHANO. He that dies pays all debts. I defy thee. Mercy
    upon us!
  CALIBAN. Art thou afeard?
  STEPHANO. No, monster, not I.
  CALIBAN. Be not afeard. The isle is full of noises,
    Sounds, and sweet airs, that give delight, and hurt not.
    Sometimes a thousand twangling instruments
    Will hum about mine ears; and sometimes voices,
    That, if I then had wak'd after long sleep,
    Will make me sleep again; and then, in dreaming,
    The clouds methought would open and show riches
    Ready to drop upon me, that, when I wak'd,
    I cried to dream again.
  STEPHANO. This will prove a brave kingdom to me, where I
    shall have my music for nothing.
  CALIBAN. When Prospero is destroy'd.
  STEPHANO. That shall be by and by; I remember the story.
  TRINCULO. The sound is going away; let's follow it, and  
    after do our work.
  STEPHANO. Lead, monster; we'll follow. I would I could see
    this taborer; he lays it on.
  TRINCULO. Wilt come? I'll follow, Stephano.             Exeunt" )",
    {1.0f, 1.0f, 1.0f, 1.0f}, 18, 16, currentY, 128, 128, "BigText", &e2);

  textDisplayEntity.set<ui::ecs::QuadRendererComponent>({
    .color = {0.3f, 0.3f, 0.3f, 1.0f},
    .borderWidths = {3.0f, 3.0f, 3.0f, 3.0f},
  });

  auto textComponent = textDisplayEntity.get_ref<ui::TextComponent>();
  textComponent->horizontalAlignment = ui::TextHAlignment_Left;
  textComponent->verticalAlignment = ui::TextVAlignment_Top;
  textComponent->isScrollable = true;
  /* ---------------------- */

  const auto e3 = ui::ecs::createEntity(&window->ecsRoot, 0, 0, 50, 50, "Entity3",
                                        &window->canvas.entity);
  e3.add<ui::LayoutComponent>();

  auto secondaryLayout = e3.get_ref<ui::LayoutComponent>();

  secondaryLayout->type = ui::LayoutType_Vertical;
  secondaryLayout->spacing = 10;

  const auto e4 = ui::ecs::createEntity(&window->ecsRoot, 0, 0, 50, 50, "Entity4", &e3);
  e4.set<ui::LayoutComponent>({
    .type = ui::LayoutType_Vertical,
    .margins = {5, 5, 5, 5},
    .spacing = 3,
  });

  /* ---------------- Create buttons ---------------- */
  constexpr size_t BUTTON_COUNT = 5;

  std::vector<std::string> buttonEntityNames(BUTTON_COUNT);
  std::vector<std::string> buttonLabels(BUTTON_COUNT);

  for (size_t i = 0; i < BUTTON_COUNT; ++i) {
    const size_t canonicalIndex = i + 1;

    buttonEntityNames[i] = "ButtonEntity" + std::to_string(canonicalIndex);
    buttonLabels[i] = "Button " + std::to_string(canonicalIndex);

    const auto button = ui::ecs::createEntity(&window->ecsRoot, 0, 0, 50, 50,
                                              buttonEntityNames[i].c_str(), &e4);

    button.set<ui::ecs::QuadRendererComponent>({
      .color = {0.36f, 0.36f, 0.36f, 1.0f},
      .borderRadius = {4, 4, 4, 4},
      .borderWidths = {2.0f, 2.0f, 2.0f, 2.0f},
    });
    button.add<ui::ecs::HoverHandlerComponent>();
    button.set<ui::ecs::ButtonComponent>({.onClick = [](const ui::ecs::Entity &entity) {
      UI_LOG_MSG("Button Clicked: '%s'", entity.name().c_str());
    }});

    button.set<ui::TextComponent>({.text = buttonLabels[i].c_str(),
                                   .font = &fontData,
                                   .color = {0.94f, 0.94f, 0.94f, 1.0f},
                                   .pixelSize = 14,
                                   .horizontalAlignment = ui::TextHAlignment_Center,
                                   .verticalAlignment = ui::TextVAlignment_Middle});

    auto buttonBaseComponent = button.get_ref<ui::ecs::BaseComponent>();
    buttonBaseComponent->inLayout = true;
    buttonBaseComponent->minHeight = 30;
    buttonBaseComponent->maxHeight = 30;
    buttonBaseComponent->minWidth = 75;
    buttonBaseComponent->maxWidth = 75;
  }

  // Add spacer
  ui::ecs::createEntity(&window->ecsRoot, 0, 0, 50, 50, "Spacer", &e4);
  /* ----------------------------------------------- */

  const auto e5 = ui::ecs::createEntity(&window->ecsRoot, 0, 0, 50, 50, "Entity5", &e3);
  const auto e6 = ui::ecs::createEntity(&window->ecsRoot, 0, 0, 50, 50, "Entity6", &e3);

  auto e6Base = e6.get_ref<ui::ecs::BaseComponent>();

  e6Base->minHeight = 100;
  e6Base->maxHeight = 100;
  e6Base->minWidth = 100;
  e6Base->maxWidth = 100;

  constexpr ui::ecs::Color red = {0.25f, 0.25f, 0.25f, 1.0f};
  constexpr ui::ecs::Color green = {0.1f, 0.1f, 0.1f, 1.0f};
  constexpr ui::ecs::Color blue = {0.75f, 0.75f, 1.75f, 1.0f};
  constexpr ui::ecs::Color white = {0.5f, 0.5f, 0.5f, 1.0f};

  e1.set<ui::ecs::QuadRendererComponent>({
    .color = red,
    .borderRadius = {30, 30, 30, 30},
    .borderColor = white,
    .borderWidths = {15.0f, 15.0f, 15.0f, 15.0f},
  });
  e4.set<ui::ecs::QuadRendererComponent>({.color = blue,
                                          .borderRadius = {9, 9, 9, 9},
                                          .borderColor = {0.25f, 0.25f, 0.25f, 1.0f},
                                          .borderWidths = {0.0f, 0.0f, 0.0f, 0.0f}});
  e5.add<ui::ecs::QuadRendererComponent>();
  e6.add<ui::ecs::QuadRendererComponent>();
  e5.get_ref<ui::ecs::QuadRendererComponent>()->color = white;
  e6.get_ref<ui::ecs::QuadRendererComponent>()->color = red;
  /* --------------------------------------------------------------------- */

  // FIXME: Currently user must call Window::relayout for input events to work
  ui::Window::relayout(window, DEMO_WINDOW_WIDTH, DEMO_WINDOW_HEIGHT);
  ui::Window::relayout(window2, 512, 512);

  auto framerateTextComponent = framerateEntity.get_ref<ui::TextComponent>();
  auto lastFrameTime = std::chrono::high_resolution_clock::now();

  while (ui::Application::update(&app)) {
    auto currentFrameTime = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> deltaTime = currentFrameTime - lastFrameTime;
    lastFrameTime = currentFrameTime;

    auto fps = static_cast<uint32_t>(1.0 / deltaTime.count());
    std::string fpsString = "FPS:" + std::to_string(fps);

    framerateTextComponent->text = fpsString.c_str();
  }

  return EXIT_SUCCESS;
}
