#include "ScoreScreenCommon.h"
#include "../../Actor/ActorTemplates.h"
#include "../../Actor/Components/RenderComponent.h"
#include "../../Actor/Actor.h"

const EventType EventData_ScoreScreen_Level_Score_Added::sk_EventType(0xac1eff1b);
const EventType EventData_Finished_Loading_Row::sk_EventType(0xb71eff1b);

//=================================================================================================
// Common functions for score screen
//=================================================================================================

StrongActorPtr SpawnImageActor(const std::string& imagePath, const Point& position, const AnimationDef& aniDef)
{
    ActorPrototype proto = ActorPrototype_StaticImage;
    if (aniDef.hasAnimation)
    {
        assert(aniDef.isCycleAnimation && "Supporting only cycle animation at this moment");

        proto = ActorPrototype_StaticAnimatedImage;
    }

    return ActorTemplates::CreateActor_StaticImage(
        proto,
        position,
        imagePath,
        aniDef);
}

void SetActorImage(Actor* pActor, const std::string& image)
{
    assert(pActor != NULL);
    assert(!image.empty());

    shared_ptr<ActorRenderComponent> pARC = MakeStrongPtr(pActor->GetComponent<ActorRenderComponent>());
    assert(pARC != nullptr);

    pARC->SetImage(image);
}

// Helper function, reflects new number to actor images
void UpdateScoreImageNumbers(int newNumber, ActorList& imageNumberActorList)
{
    std::string dividerStr(imageNumberActorList.size(), '0');
    dividerStr[0] = '1';
    int divider = std::stoi(dividerStr);

    for (Actor* pScoreNumber : imageNumberActorList)
    {
        int num = (newNumber / divider) % 10;
        // All score images are automatically converted to frame00X format where frame001 is 0, frame002 is 1 etc.
        std::string imageNumStr = "frame" + Util::ConvertToThreeDigitsString(num + 1);
        SetActorImage(pScoreNumber, imageNumStr);

        divider /= 10;
    }
}

void AddNumberImageActorsToList(int numberToDisplay, int futureMaximumNumber, Point position, ActorList& toList)
{
    std::string numberStr = ToStr(numberToDisplay);
    std::string maximumNumberStr = ToStr(futureMaximumNumber);
    if (numberStr.length() != maximumNumberStr.length())
    {
        assert(maximumNumberStr.length() > numberStr.length());
        numberStr.reserve(maximumNumberStr.length());
        int zerosToAdd = maximumNumberStr.length() - numberStr.length();
        for (int i = 0; i < zerosToAdd; i++)
        {
            numberStr.insert(0, "0");
        }
    }

    toList.reserve(toList.size() + numberStr.length());
    for (char charDigit : numberStr)
    {
        int digit = DigitCharToInt(charDigit);
        std::string numberImagePath;

        numberImagePath = "/STATES/BOOTY/IMAGES/SCORENUMBERS/*.PID";

        StrongActorPtr pNumberActor = SpawnImageActor(numberImagePath, position);

        toList.push_back(pNumberActor.get());

        const Point numberOffset(12, 0);

        position += numberOffset;
    }
}

int DigitCharToInt(char c)
{
    assert('0' <= c && c <= '9');
    return c - '0';
}
