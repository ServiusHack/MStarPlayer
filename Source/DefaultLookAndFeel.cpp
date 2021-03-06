#include "DefaultLookAndFeel.h"

DefaultLookAndFeel::DefaultLookAndFeel()
{
    // Fix what LookAndFeelV3 broke in 4.2.0:
    setColour(juce::TextButton::buttonOnColourId, juce::Colour(0xff4444ff));
}
void DefaultLookAndFeel::drawProgressBar(juce::Graphics& g, juce::ProgressBar& progressBar, int width, int height,
    double progress, const juce::String& textToShow)
{
    const juce::Colour background(progressBar.findColour(juce::ProgressBar::backgroundColourId));
    const juce::Colour foreground(progressBar.findColour(juce::ProgressBar::foregroundColourId));

    g.fillAll(background);

    if (progress >= 0.0f && progress < 1.0f)
    {
        drawGlassLozenge(g,
            1.0f,
            1.0f,
            (float)juce::jlimit(0.0, width - 2.0, progress * (width - 2.0)),
            (float)(height - 2),
            foreground,
            0.5f,
            0.0f,
            true,
            true,
            true,
            true);
    }
    else
    {
        // spinning bar..
        g.setColour(foreground);

        const int stripeWidth = height * 2;
        const int position = (int)(juce::Time::getMillisecondCounter() / 15) % stripeWidth;

        juce::Path p;

        for (float x = (float)(-position); x < width + stripeWidth; x += stripeWidth)
            p.addQuadrilateral(
                x, 0.0f, x + stripeWidth * 0.5f, 0.0f, x, (float)height, x - stripeWidth * 0.5f, (float)height);

        juce::Image im(juce::Image::ARGB, width, height, true);

        {
            juce::Graphics g2(im);
            drawGlassLozenge(g2,
                1.0f,
                1.0f,
                (float)(width - 2),
                (float)(height - 2),
                foreground,
                0.5f,
                0.0f,
                true,
                true,
                true,
                true);
        }

        g.setTiledImageFill(im, 0, 0, 0.85f);
        g.fillPath(p);
    }

    if (textToShow.isNotEmpty())
    {
        g.setColour(juce::Colours::black);
        g.setFont(height * 0.6f);

        g.drawText(textToShow, 0, 0, width, height, juce::Justification::centred, false);
    }
}

void DefaultLookAndFeel::drawLevelMeter(juce::Graphics& g, int width, int height, float level, float peak)
{
    g.setColour(juce::Colours::white.withAlpha(0.7f));
    g.fillRect(0.0f, 0.0f, (float)width, (float)height);
    g.setColour(juce::Colours::black.withAlpha(0.2f));
    g.fillRect(1.0f, 1.0f, width - 2.0f, height - 2.0f);

    const int totalBlocks = 20;
    const int numBlocks = juce::roundToInt(totalBlocks * level);
    const int peakBlock = juce::roundToInt(totalBlocks * peak) - 1;
    const float w = (width - 6.0f) / (float)totalBlocks;

    for (int i = 0; i < totalBlocks; ++i)
    {
        if (i >= numBlocks && i != peakBlock)
            g.setColour(juce::Colours::lightblue.withAlpha(0.6f));
        else if (i >= totalBlocks - 1)
            g.setColour(juce::Colours::maroon);
        else if (i >= totalBlocks - 5)
            g.setColour(juce::Colours::gold);
        else
            g.setColour(juce::Colours::blue.withAlpha(0.5f));

        g.fillRect(3.0f + i * w + w * 0.1f, 3.0f, w * 0.8f, height - 6.0f);
    }
}
